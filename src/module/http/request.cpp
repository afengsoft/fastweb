﻿#include "request.h"
#include "net/http_reqpack.h"
#include "session.h"
module::request::request(network::http::request* request) :m_request(request)
{
}
module::request::~request()
{
    if (m_session != nullptr)
        delete m_session;
}
module::session* module::request::session(const std::string& token)
{
    if (m_session == nullptr)
        m_session = new module::session(&m_request->session(token));
    return m_session;
}
sol::table module::request::body_param(sol::this_state s)
{
    sol::state_view lua(s);
    sol::table result_table = lua.create_table();
    auto map = m_request->parser()->body_param();
    for_iter(iter,map)
        result_table[iter->first] = iter->second;
    return result_table;
}
sol::table module::request::url_param(sol::this_state s)
{
    sol::state_view lua(s);
    sol::table result_table = lua.create_table();
    auto map = m_request->parser()->url_param();
    for_iter(iter, map)
        result_table[iter->first] = iter->second;
    return result_table;
}
std::string module::request::body()
{
    return m_request->parser()->text();
}
void* module::request::website()
{
    return m_request->website();
}
sol::table module::request::files(sol::this_state s)
{
    auto names = m_request->parser()->form()->names();
    sol::state_view lua(s);
    sol::table result_table = lua.create_table();

    int count = 1;
    for (size_t i = 0; i < names.size(); i++)
    {
        sol::table file = lua.create_table();
        network::http::form_info fi;
        if (m_request->parser()->form()->get(names[i], fi))
        {
            file["type"] = fi.content_type;
            file["size"] = fi.data.size();
            file["filename"] = fi.filename;
            result_table[count] = file;
            count++;
        }
    }
    return result_table;
}
bool module::request::write_file(const std::string& name, const std::string& filepath)
{
    return m_request->parser()->form()->write_file(name,filepath);
}
void module::request::regist(sol::state* lua)
{
    // 绑定 Request 类到 Lua
    lua->new_usertype<module::request>("module_request",
        "header", &module::request::header,
        "method", &module::request::method,
        "filepath", &module::request::filepath,
        "host", &module::request::host,
        "remote_ipaddress", &module::request::remote_ipaddress,
        "remote_port", &module::request::remote_port,
        "param", &module::request::param,
        "session", &module::request::session,
        "body_param", &module::request::body_param,
        "url_param", &module::request::url_param,
        "body", &module::request::body,
        "files", &module::request::files,
        "write_file", &module::request::write_file
    );
    (*lua)["GET"] = (int)network::http::GET;
    (*lua)["POST"] = (int)network::http::POST;
    (*lua)["DEL"] = (int)network::http::DEL;
    (*lua)["HEAD"] = (int)network::http::HEAD;
    (*lua)["PUT"] = (int)network::http::PUT;
}


std::string module::request::header(const std::string& name)
{
    std::string value;
    m_request->header(name, value);
    return value;
}
ylib::network::http::method module::request::method()
{
    return m_request->method();
}
std::string module::request::filepath()
{
    return m_request->filepath();
}
std::string module::request::host()
{
    return m_request->host();
}
sol::object module::request::param(const std::string& name, bool throw_,sol::this_state s)
{
    std::string value;
    bool result = request_param(name, value);
    if (result == false){
        if (throw_)
            throw ylib::exception("request parameter '" + name + "' was not found");
        else
            return sol::make_object(s, sol::nil);
    }
    return sol::make_object(s,value);
}

std::string module::request::remote_ipaddress()
{
    return m_request->remote_ipaddress(false);
}
ushort module::request::remote_port()
{
    return m_request->remote_port();
}
bool module::request::request_param(const std::string& name, std::string& value)
{
    if (m_request->parser()->url_param(name, value) == false)
        return m_request->parser()->body_param(name, value);
    return true;
}
