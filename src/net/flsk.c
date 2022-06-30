#include"flsk.h"

struct flskDVWS_;
struct flskdvwsConnection;

static void flskdvwsRun(flSocket* sok);
static size_t flskdvwsSend(flSocket* sok, flskMessage* msg);
static void flskdvwsClosec(flSocket* sok, flskMessage* msg);
static void flskdvwsFree(flSocket* sok);
static void flskdvwsMgEventCb(struct mg_connection *c, int ev, void *ev_data, void *fn_data);

//Add the given connection to the connections list if doesn't already exist.
static void flskdvwsAddCon(flSocket* sok, struct flskdvwsConnection con);

static struct flskdvwsConnection* flskdvwsGetCon(flSocket* sok, flskID_t key);

struct flskdvwsConnection{
    flskID_t dvKey;
    struct mg_connection* mgCon;
};

struct flskDVWS_{
    struct mg_mgr conMgr;
    flArray* cons;
    const char* rootDir;
};

flSocket* flskdvwsNew(const char* rootDir, const char* url, flskRecvCb_tf recvCb){
    flSocket* sok = NULL;

    //->dvws
    struct flskDVWS_* dvws = malloc(sizeof(struct flskDVWS_ ));
    if(!dvws) goto ERR_EXIT;

    dvws->cons = flarrNew(5, sizeof(struct flskdvwsConnection));
    if(!dvws->cons) goto ERR_EXIT;

    dvws->rootDir = rootDir;

    //->sok
    sok = malloc(sizeof(flSocket));
    if(!sok) goto ERR_EXIT;

    *sok = (flSocket){ flSocket_, 
        ._run = flskdvwsRun, ._send = flskdvwsSend, ._closec = flskdvwsClosec,
        ._free = flskdvwsFree, .recvCb = recvCb, ._ = dvws };

    //->Initialize websocket server
    mg_mgr_init(&dvws->conMgr);// Initialise event manager
    if(!mg_http_listen(&dvws->conMgr, url, flskdvwsMgEventCb, sok)){ // Create HTTP listener
        mg_mgr_free(&dvws->conMgr);
        goto ERR_EXIT;
    }

    return sok;

ERR_EXIT:
    if(dvws && dvws->cons) flarrFree(dvws->cons);
    if(dvws) free(dvws);
    if(sok) free(sok);

    return NULL;
}

void flskdvwsRun(flSocket* sok){
    mg_mgr_poll( &((struct flskDVWS_*)sok->_)->conMgr, 1000/*timeout in ms*/);
}

size_t flskdvwsSend(flSocket* sok, flskMessage* msg){
    struct flskdvwsConnection* c = flskdvwsGetCon(sok, flskGetDeviceKey(sok, msg->data));
    if(c){
        return mg_ws_send(c->mgCon, msg->data, msg->dataLen, WEBSOCKET_OP_BINARY);
    }
}

void flskdvwsClosec(flSocket* sok, flskMessage* msg){
    struct flskdvwsConnection* c = flskdvwsGetCon(sok, flskGetDeviceKey(sok, msg->data));
    if(c){
        mg_ws_send(c->mgCon, msg->data, msg->dataLen, WEBSOCKET_OP_BINARY);
        
        c->mgCon->is_draining = 1;
        *c = (struct flskdvwsConnection){.dvKey = 0, .mgCon = NULL};
    }
}

void flskdvwsFree(flSocket* sok){
    if(!sok) return;

    if(sok->_){
        struct flskDVWS_* dvws = sok->_;

        if(dvws->cons){
            flarrFree(dvws->cons);
            dvws->cons = NULL;
        }
        mg_mgr_free(&dvws->conMgr);

        free(dvws);
        sok->_ = NULL;
    }

    free(sok);
}

static void flskdvwsMgEventCb(struct mg_connection *c, int ev, void *ev_data, void *fn_data){
    flSocket* sok = fn_data;
    struct flskDVWS_* dvws = sok->_;

    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        if (mg_http_match_uri(hm, "/websocket")) {
            // Upgrade to websocket. From now on, a connection is a full-duplex
            // Websocket connection, which will receive MG_EV_WS_MSG events.
            mg_ws_upgrade(c, hm, NULL);
        }else{
            struct mg_http_serve_opts opts = {.root_dir = dvws->rootDir};
            mg_http_serve_dir(c, ev_data, &opts);
        }
    }else if (ev == MG_EV_WS_MSG) {
        struct mg_ws_message* wm = (struct mg_ws_message*)ev_data;

        flskdvwsAddCon(sok, 
            (struct flskdvwsConnection){.dvKey = flskGetDeviceKey(sok, wm->data.ptr), .mgCon = c});

        sok->recvCb(sok, (flskMessage){flskMessage_, .data = wm->data.ptr, .dataLen = wm->data.len});

        mg_iobuf_delete(&c->recv, c->recv.len);
    }
}

static void flskdvwsAddCon(flSocket* sok, struct flskdvwsConnection con){
    struct flskDVWS_* dvws = sok->_;
    int nullIndex = -1;

    for(int i = 0; i < dvws->cons->length; i++){
        struct flskdvwsConnection* c = flarrGet(dvws->cons, i);

        if(!c->mgCon){
            nullIndex = i;
            continue;
        }

        if(c->dvKey == con.dvKey){
            if(c->mgCon == con.mgCon) return;
            //close the existing connection, the new connection will be added later
            c->mgCon->is_draining = 1;
            c->mgCon = NULL; c->dvKey = 0;

            nullIndex = i;
        }
    }

    //Add the new connection
    if(nullIndex == -1) flarrPush(dvws->cons, &con);
    else flarrPut(dvws->cons, nullIndex, &con);
}

struct flskdvwsConnection* flskdvwsGetCon(flSocket* sok, flskID_t key){
    struct flskDVWS_* dvws = sok->_;

    for(int i = 0; i < dvws->cons->length; i++){
        struct flskdvwsConnection* c = flarrGet(dvws->cons, i);
        if(c->mgCon && c->dvKey == key) return c;
    }   

    return NULL;
}