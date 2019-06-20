#include <stdio.h>
#include <string>
#include <stdint.h>
// #include "zookeeper.h"
#include "hiredis/adapters/libevent.h"
#include <signal.h>
#include "hiredis/hiredis.h"
#include "hiredis/async.h"
#include "event2/event.h"
#include <google/protobuf/message.h>
#include <functional>
// void init_watch_fn(zhandle_t *zh, int type, 
        // int state, const char *path,void *watcherCtx)
// {
	
// } 
class redis_example
{
public:
static void onMessage(redisAsyncContext *c, void *reply, void *privdata) {
    if (c->err) {
    	printf("%s\n", c->errstr);
    	exit(0);
    }

    if (reply == NULL) {
    	return;
    }

    redisReply *r = (redisReply*)reply;
    if (r->type == REDIS_REPLY_ERROR) {
        printf("%s\n", r->str);
        exit(0);
    }

    if (privdata) {
        auto key = (std::string*)privdata;
        printf("privdata:%s\n", key->c_str());
    }

    if (r->type == REDIS_REPLY_ARRAY) {
        for (int j = 0; j < r->elements; j++) {
            printf("%u) %s\n", j, r->element[j]->str);
        }
    }
    if (r->type == REDIS_REPLY_INTEGER) {
        printf("int:%u\n", r->integer);
    }
    if (r->type == REDIS_REPLY_STRING) {
        printf("str:%s\n", r->str);
    }
}

void redis_pubsub()
{
    signal(SIGPIPE, SIG_IGN);
    struct event_base *base = event_base_new();

    redisAsyncContext *c = redisAsyncConnect("127.0.0.1", 6379);
    if (c->err) {
        printf("error: %s\n", c->errstr);
        return ;
    }

    redisLibeventAttach(c, base);
    redisAsyncCommand(c, redis_example::onMessage, NULL, "SUBSCRIBE testtopic");
    event_base_dispatch(base);	
}

int redislist()
{
	redisContext* c = redisConnect("127.0.0.1", 6379);
	if (!c || c->err) {
		printf("error\n");
		return 0;
	}
	redisReply* reply = static_cast<redisReply*>(redisCommand(c, "BLPOP testlist 0"));
	if (!reply) {
		if (c->err) {
			printf("%s\n", c->errstr);
			return 0;
		}
	}
	
	if (reply->type == REDIS_REPLY_ARRAY) {
        for (int j = 0; j < reply->elements; j++) {
            printf("%u) %s\n", j, reply->element[j]->str);
        }
    }	
}

void rediskey()
{
    signal(SIGPIPE, SIG_IGN);
    struct event_base *base = event_base_new();

    redisAsyncContext *c = redisAsyncConnect("127.0.0.1", 6379);
    if (c->err) {
        printf("error: %s\n", c->errstr);
        return ;
    }

    redisLibeventAttach(c, base);

    std::string* key1 = new std::string("test_key");
    std::string* key2 = new std::string("test2_key");

    redisAsyncCommand(c, redis_example::onMessage, key1, "AUTH 1234");
    redisAsyncCommand(c, redis_example::onMessage, key1, "GET test_key");
    redisAsyncCommand(c, redis_example::onMessage, key2, "GET test_key2");
    event_base_dispatch(base);      
}

};

class test_destruction
{
public:
    test_destruction()
    {
        printf("construct\n");
    }
    test_destruction(const test_destruction& td)
    {
        printf("copy\n");
    }
    void operator= ( test_destruction& td)
    {
        printf("move\n");
    }
    ~test_destruction()
    {
        printf("destruction\n");
    }
    static test_destruction construct()
    {
        return test_destruction();
    }
};

class test_bind
{
public:
    void fn(int i, int j)
    {

    }
};


int main(int argc, char const *argv[])
{
    using namespace std::placeholders;
    test_bind tb;
    void (*fn)(int, int) = std::bind(&test_bind::fn, &tb, _1, _2);
	// redis_example re;
	// re.rediskey();
    // test_destruction::construct();
	return 0;
}