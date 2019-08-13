#include <stdio.h>
#include <string>
#include <iostream>
#include <stdint.h>
#include "test.pb.h"
#include <google/protobuf/descriptor.h>
// #include "zookeeper.h"
// #include "hiredis/adapters/libevent.h"
#include <signal.h>
// #include "hiredis/hiredis.h"
// #include "hiredis/async.h"
// #include "event2/event.h"
#include <google/protobuf/message.h>
#include <functional>
#include <chrono>
#include <thread>
#include <cstring>
#include <unordered_map>
#include <random>
#include <set>
using namespace std::literals::chrono_literals;
// void init_watch_fn(zhandle_t *zh, int type, 
        // int state, const char *path,void *watcherCtx)
// {
	
// } 
// class redis_example
// {
// public:
//     static bool need_unsub;
//     static bool need_resub;
//     static bool need_punsub;
//     static bool need_presub;
//     static void onMessage(redisAsyncContext *c, void *reply, void *privdata) 
//     {
//         if (c->err) {
//         	printf("%s\n", c->errstr);
//         	exit(0);
//         }

//         if (reply == NULL) {
//         	return;
//         }

//         redisReply *r = (redisReply*)reply;
//         if (r->type == REDIS_REPLY_ERROR) {
//             printf("%s\n", r->str);
//             exit(0);
//         }

//         if (privdata) {
//             auto key = (std::string*)privdata;
//             printf("privdata:%s\n", key->c_str());
//         }

//         if (r->type == REDIS_REPLY_ARRAY) {
//             for (int j = 0; j < r->elements; j++) {
//                 printf("%u) %s\n", j, r->element[j]->str);
//                 if (std::strncmp(r->element[j]->str, "unsubscribe", sizeof("unsubscribe")) == 0) {
//                     need_resub = true;
//                     return;
//                 }
//                 if (std::strncmp(r->element[j]->str, "punsubscribe", sizeof("punsubscribe")) == 0) {
//                     need_presub = true;
//                     return;
//                 }
//                 if (std::strncmp(r->element[j]->str, "subscribe", sizeof("subscribe")) == 0) {
//                     return;
//                 }
//                 if (std::strncmp(r->element[j]->str, "psubscribe", sizeof("psubscribe")) == 0) {
//                     return;
//                 }
//                 if (std::strncmp(r->element[j]->str, "message", sizeof("message")) == 0) {
//                     need_unsub = true;
//                     return;
//                 }
//                 if (std::strncmp(r->element[j]->str, "pmessage", sizeof("pmessage")) == 0) {
//                     need_punsub = true;
//                     return;
//                 }
//             }
//         }
//         if (r->type == REDIS_REPLY_INTEGER) {
//             printf("int:%u\n", r->integer);
//         }
//         if (r->type == REDIS_REPLY_STRING) {
//             printf("str:%s\n", r->str);
//         }

//         // need_unsub = true;
//         // redisAsyncCommand(c, , nullptr, "UNSUBSCRIBE testtopic");
//     };

//     static void get_1(redisAsyncContext *c, void *reply, void *privdata) 
//     {
//         if (c->err) {
//             printf("%s\n", c->errstr);
//             exit(0);
//         }

//         if (reply == NULL) {
//             return;
//         }
//         printf("get_1\n");

//         redisReply *r = (redisReply*)reply;
//         if (r->type == REDIS_REPLY_ERROR) {
//             printf("%s\n", r->str);
//             exit(0);
//         }

//         if (privdata) {
//             auto key = (std::string*)privdata;
//             printf("privdata:%s\n", key->c_str());
//         }

//         if (r->type == REDIS_REPLY_ARRAY) {
//             for (int j = 0; j < r->elements; j++) {
//                 printf("%u) %s\n", j, r->element[j]->str);
//             }
//         }
//         if (r->type == REDIS_REPLY_INTEGER) {
//             printf("int:%u\n", r->integer);
//         }
//         if (r->type == REDIS_REPLY_STRING) {
//             printf("str:%s\n", r->str);
//         }
//     };

//     static void get_2(redisAsyncContext *c, void *reply, void *privdata) 
//     {
//         if (c->err) {
//             printf("%s\n", c->errstr);
//             exit(0);
//         }

//         if (reply == NULL) {
//             return;
//         }
//         printf("get_2\n");

//         redisReply *r = (redisReply*)reply;
//         if (r->type == REDIS_REPLY_ERROR) {
//             printf("%s\n", r->str);
//             exit(0);
//         }

//         if (privdata) {
//             auto key = (std::string*)privdata;
//             printf("privdata:%s\n", key->c_str());
//         }

//         if (r->type == REDIS_REPLY_ARRAY) {
//             for (int j = 0; j < r->elements; j++) {
//                 printf("%u) %s\n", j, r->element[j]->str);
//             }
//         }
//         if (r->type == REDIS_REPLY_INTEGER) {
//             printf("int:%u\n", r->integer);
//         }
//         if (r->type == REDIS_REPLY_STRING) {
//             printf("str:%s\n", r->str);
//         }
//     };

//     static void on_unsubscribe(redisAsyncContext *c, void *reply, void *privdata)
//     {
//         if (c->err) {
//             printf("%s\n", c->errstr);
//             exit(0);
//         }

//         if (reply == NULL) {
//             return;
//         }

//         redisReply *r = (redisReply*)reply;
//         if (r->type == REDIS_REPLY_ERROR) {
//             printf("%s\n", r->str);
//             exit(0);
//         }

//         if (r->type == REDIS_REPLY_ARRAY) {
//             for (int j = 0; j < r->elements; j++) {
//                 printf("unsubscribe %u %s\n", j, r->element[j]->str);
//                 // if (std::strncmp(r->element[j]->str, "unsubscribe", sizeof("unsubscribe")) == 0) {
//                 //     return;
//                 // }
//                 // if (std::strncmp(r->element[j]->str, "subscribe", sizeof("unsubscribe")) == 0) {
//                 //     return;
//                 // }
//             }
//         }
//         // redisAsyncCommand(c, redis_example::onMessage, NULL, "SUBSCRIBE testtopic");
//         need_resub = true;
//     }

//     int redislist()
//     {
//     	redisContext* c = redisConnect("127.0.0.1", 6379);
//     	if (!c || c->err) {
//     		printf("error\n");
//     		return 0;
//     	}
//     	redisReply* reply = static_cast<redisReply*>(redisCommand(c, "BLPOP testlist 0"));
//     	if (!reply) {
//     		if (c->err) {
//     			printf("%s\n", c->errstr);
//     			return 0;
//     		}
//     	}
    	
//     	if (reply->type == REDIS_REPLY_ARRAY) {
//             for (int j = 0; j < reply->elements; j++) {
//                 printf("%u) %s\n", j, reply->element[j]->str);
//             }
//         }	
//     };

//     void rediskey()
//     {
//         signal(SIGPIPE, SIG_IGN);
//         struct event_base *base = event_base_new();

//         redisAsyncContext *c = redisAsyncConnect("127.0.0.1", 6379);
//         if (c->err) {
//             printf("error: %s\n", c->errstr);
//             return ;
//         }

//         redisLibeventAttach(c, base);

//         std::string* key1 = new std::string("test_key");
//         std::string* key2 = new std::string("test2_key");

//         // redisAsyncCommand(c, redis_example::onMessage, key1, "AUTH 1234");
//         redisAsyncCommand(c, redis_example::get_1, key1, "GET test_key");
//         redisAsyncCommand(c, redis_example::get_2, key2, "GET test_key2");
//         event_base_dispatch(base);      
//     }

//     void redis_pubsub()
//     {
//         signal(SIGPIPE, SIG_IGN);
//         struct event_base *base = event_base_new();

//         redisAsyncContext *c = redisAsyncConnect("127.0.0.1", 6379);
//         if (c->err) {
//             printf("error: %s\n", c->errstr);
//             return ;
//         }

//         // redisAsyncSetConnectCallback(sub_rac, handle_sub_connected);
//         redisLibeventAttach(c, base);
//         redisAsyncCommand(c, redis_example::onMessage, NULL, "SUBSCRIBE testtopic");
//         redisAsyncCommand(c, redis_example::onMessage, NULL, "UNSUBSCRIBE testtopic");
//         redisAsyncCommand(c, redis_example::onMessage, NULL, "PSUBSCRIBE testpsub*");
//         // redisAsyncCommand(c, redis_example::onMessage, NULL, "SUBSCRIBE remain_sub");
//         // event_base_dispatch(base);  
//         while (true) {
//             event_base_loop(base, EVLOOP_NONBLOCK);
//             if (need_unsub) {
//                 redisAsyncCommand(c, on_unsubscribe, nullptr, "UNSUBSCRIBE testtopic");
//                 // redisAsyncCommand(c, onMessage, nullptr, "SUBSCRIBE testtopic");
//                 need_unsub = false;
//             }

//             if (need_resub) {
//                 redisAsyncCommand(c, onMessage, nullptr, "SUBSCRIBE testtopic");
//                 need_resub = false;
//             }

//             if (need_punsub) {
//                 redisAsyncCommand(c, on_unsubscribe, nullptr, "PUNSUBSCRIBE testpsub*");
//                 // redisAsyncCommand(c, redis_example::onMessage, NULL, "PSUBSCRIBE testpsub*");
//                 need_punsub = false;
//             }
//             if (need_presub) {
//                 redisAsyncCommand(c, redis_example::onMessage, NULL, "PSUBSCRIBE testpsub*");
//                 need_presub = false;
//             }
//             std::this_thread::sleep_for(1s);
//         }
//     };

// };
// bool redis_example::need_unsub = false;
// bool redis_example::need_resub = false;
// bool redis_example::need_punsub = false;
// bool redis_example::need_presub = false;
// class test_destruction
// {
// public:
//     test_destruction()
//     {
//         printf("construct\n");
//     }
//     test_destruction(const test_destruction& td)
//     {
//         printf("copy\n");
//     }
//     void operator= ( test_destruction& td)
//     {
//         printf("move\n");
//     }
//     ~test_destruction()
//     {
//         printf("destruction\n");
//     }
//     static test_destruction construct()
//     {
//         return test_destruction();
//     }
// };

// template <typename T>
// void test_func_template()
// {
//     struct test_struct_t
//     {
//         T t;
//         std::unordered_map<T, int> map;
//     } ;
//     static test_struct_t ts;
// };

// #define REG_SERVICE(service_name) \
//     extern "C" \
//     { \
//         void service_name##_init_func(); \
//         void service_name##_tick_func(); \
//         void service_name##_end_func(); \
//         void service_name##_service_process_cmd(std::string& cmd_name, std::string& cmd_data); \
//     }

// REG_SERVICE(test)

using namespace google;
using namespace protobuf;
#include <sstream>
int main(int argc, char const *argv[])
{
    test::test_reflection msg;
    // msg.set_integer(1);
    msg.add_int_list(100);
    std::vector< const FieldDescriptor * > field_list;
    auto reflection = msg.GetReflection();
    reflection->ListFields(msg, &field_list);

    test::test_reflection msg2;
    for (const auto it : field_list) {
        printf("%s\n", it->name().c_str());
        if (it->is_repeated() && it->type() == FieldDescriptor::TYPE_INT32) {
            int size = reflection->FieldSize(msg, it);
            for (int i = 0; i < size; ++i) {
                int value = reflection->GetRepeatedInt32(msg, it, i);
                printf("field_value:%d %d\n", i, value);
                reflection->AddInt32(&msg2, it, value);
            }
        }
        // if (it->type() == FieldDescriptor::TYPE_INT32) {
            // reflection->SetInt32(&msg2, it, 100);
        // }
    }


    // for (const auto it : field_list) {
        // int32_t ret = reflection->GetInt32(msg2, it);
    // }
    // printf("%u\n", msg.integer());
    printf("%s\n", msg2.ShortDebugString().c_str());

    return 0;
}
