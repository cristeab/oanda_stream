#include <iostream>
#include <algorithm>
#include <cpprest/http_client.h>

//#define API_TYPE "trade"
#define API_TYPE "practice"
#define API_URL "https://api-fx" API_TYPE ".oanda.com"
#define STREAMING_API_URL "https://stream-fx" API_TYPE ".oanda.com"
#define API_KEY "ca1d6eace912d78b2ce9914e5c1908b0-af85787edbf26e387a65ece570801de6"

typedef std::vector<uint8_t> bytes;

static
utility::string_t account_id;

static
void parse_list_accounts(web::json::value v)
{
    if (!v.is_null() && v.is_object()) {
        auto obj_val = v.as_object().at(U("accounts"));
        if (obj_val.is_array()) {
            const auto arr = obj_val.as_array();
            for (const auto &arr_val: arr) {
                account_id = arr_val.at(U("id")).serialize();
                //remove quotes
                account_id.erase(std::remove(account_id.begin(), account_id.end(), U('\"')),
                                 account_id.end());
                std::cout << "Account ID: " << account_id << std::endl;
                //TODO: expecting a single account
            }
        }
    }
}

static
void list_accounts()
{
    web::http::client::http_client client(U(API_URL) U("/v3/accounts"));

    web::http::http_request request(web::http::methods::GET);
    request.headers().add(U("Content-Type"), U("application/json"));
    request.headers().add(U("Authorization"), U("Bearer ") U(API_KEY));

    pplx::task<void> task = client.request(request)
            .then([](web::http::http_response response)->pplx::task<web::json::value>{
            if(response.status_code() == web::http::status_codes::OK){
            return response.extract_json();
} else {
            return pplx::task_from_result(web::json::value());
};})
            .then([](pplx::task<web::json::value> previousTask){
        try{
            const web::json::value & v = previousTask.get();
            parse_list_accounts(v);
        } catch(const web::http::http_exception &e){
            std::cout << e.what() << std::endl;
        }
    });
    try{
        task.wait();
    } catch(std::exception &e){
        std::cout << e.what() << std::endl;
    }
}

static
void subscribe_pricing_stream()
{
    web::uri_builder uri_bld;
    uri_bld.set_path(U(STREAMING_API_URL) U("/v3/accounts/") +
                         account_id + U("/pricing/stream"));
    uri_bld.append_query(U("instruments"), U("EUR_USD"));
    web::http::client::http_client client(uri_bld.to_string());

    web::http::http_request request(web::http::methods::GET);
    request.headers().add(U("Authorization"), U("Bearer ") U(API_KEY));

    pplx::task<void> task = client.request(request)
            .then([](web::http::http_response response)->pplx::task<bytes> {
            std::cout << "Response" << response.to_string() << std::endl;
            if(response.status_code() == web::http::status_codes::OK){
            return response.extract_vector();
} else {
            return pplx::task_from_result(bytes());
};})
            .then([](pplx::task<bytes> previousTask){
        try{
            const bytes & v = previousTask.get();
            std::cout << "Reply " << v.size() << std::endl;
        } catch(const web::http::http_exception &e){
            std::cout << e.what() << std::endl;
        }
    });
    try{
        task.wait();
    } catch(std::exception &e){
        std::cout << e.what() << std::endl;
    }
}

int main()
{
    list_accounts();
    subscribe_pricing_stream();
}
