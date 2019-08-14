#include <iostream>
#include <cpprest/http_client.h>

//#define API_TYPE "trade"
#define API_TYPE "practice"
#define API_URL "https://api-fx" API_TYPE ".oanda.com"
#define STREAMING_API_URL "https://stream-fx" API_TYPE ".oanda.com"
#define API_KEY "ca1d6eace912d78b2ce9914e5c1908b0-af85787edbf26e387a65ece570801de6"

static
std::string account_id;

static
void parse_list_accounts(web::json::value v)
{
    if (!v.is_null() && v.is_object()) {
        auto obj_val = v.as_object().at("accounts");
        if (obj_val.is_array()) {
            const auto arr = obj_val.as_array();
            for (const auto &arr_val: arr) {
                account_id = arr_val.at("id").serialize();
                std::cout << "Account ID: " << arr_val.at("id").serialize() << std::endl;
                //TODO: expecting a single account
            }
        }
    }
}

static
void list_accounts()
{
    web::http::client::http_client client(API_URL "/v3/accounts");

    web::http::http_request request(web::http::methods::GET);
    request.headers().add("Content-Type", "application/json");
    request.headers().add("Authorization", "Bearer " API_KEY);

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
            std::cout << "JSON reply " << v << std::endl;
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

}

int main()
{
    list_accounts();
}
