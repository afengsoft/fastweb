#pragma once
#include "base/define.h"
#include "base/error.h"

#include "util/thread.h"
#include <mutex>
#include <list>
namespace ylib
{
	/*********************************************************************
	 * class：缓存
     *********************************************************************/
    class cache:public ylib::error_base,private ylib::ithread
	{
	public:
		struct cache_info
		{
			cache_info()
			{
				update_sec = 0;
				timeout_sec = 0;
				update = false;
			}
			// 创建时间
			timestamp update_sec;
			// 过期时间
			uint32 timeout_sec;
			// 内容
            std::string value;
			// 更新
			bool update;
		};
		cache();
		~cache();
		/***************************************************************************************************
		 * function：启动
		 * param
		 *			local_path						：						缓存保存路径[默认=空]，空则不保存本地。
	     ****************************************************************************************************/
        bool start(const std::string& local_path);
		/***************************************************************************************************
		 * function：停止
		 ****************************************************************************************************/
		void stop();
		/***************************************************************************************************
		 * function：读
		 * param
		 *			name								：						键
		 *			value								：						值
		 ****************************************************************************************************/
        bool read(const std::string& name,std::string& value);
		/***************************************************************************************************
		 * function：写
		 * param
		 *			name								：						键
		 *			value								：						值
		 *			timeout_sec						：						过期时间  -2=不设置
		 ****************************************************************************************************/
        bool write(const std::string& name, const std::string& value,int32 timeout_sec = -2);
        bool write(const std::string& name, const std::string& value,timestamp update_sec,int32 timeout_sec);
		/***************************************************************************************************
		 * function：是否存在
		 * param
		 *			name								：						键
		 ****************************************************************************************************/
        bool exist(const std::string& name);
		/***************************************************************************************************
		 * function：失效时间
		 * param
		 *			name								：						键
		 * return
		 *			>=0 则为失效时间，-1=永不过期 -2=无此键
		 ****************************************************************************************************/
        int32 expire(const std::string& name);
		/***************************************************************************************************
		 * function：删除
		 * param
		 *			name								：						键
		 ****************************************************************************************************/
        bool remove(const std::string& name);
		/***************************************************************************************************
		 * function：更新过期
		 ****************************************************************************************************/
        bool update(const std::string& name);
		/***************************************************************************************************
		 * function：清空
		 ****************************************************************************************************/
		void clear();
		/***************************************************************************************************
		 * function：取MAP
		 ****************************************************************************************************/
        std::map<std::string,cache_info*> *list();
        std::mutex* mutex();
	private:
        bool _exist(const std::string& name,bool lock);
		// 通过 ithread 继承
		virtual bool run() override;
	private:
		// 本地缓存路径
        std::string m_local_path;
		// 读写锁
        std::mutex m_mutex;
		// 缓存数据
        std::map<std::string, cache_info*> m_cache;

	};
}
