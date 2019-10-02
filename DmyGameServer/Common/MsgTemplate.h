#ifndef _MSG_TEMPLATE_H_
#define _MSG_TEMPLATE_H_

template <typename T1, typename T2>
std::tuple<T1*, T2*> construct_msg()
{
	char* header = (char*)malloc(sizeof(T1) + sizeof(T2));
	T2* data = header + sizeof(T1);
	return {(T1*)header, data};
}

template <typename T1, typename T2>
std::tuple<T1*, T2*> construct_msg(size_t t2_size)
{
	char* header = (char*)malloc(sizeof(T1) + t2_size);
	T2* data = header + sizeof(T1);
	return {(T1*)header, data};
}

#endif