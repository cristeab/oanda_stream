#include <iostream>
#include <cpprest/http_client.h>

#define API_URL "https://api-fxpractice.oanda.com"
#define STREAMING_API_URL "https://stream-fxpractice.oanda.com"
#define API_KEY "ca1d6eace912d78b2ce9914e5c1908b0-af85787edbf26e387a65ece570801de6"

static
void printJSON(web::json::value v)
{
    if (!v.is_null()){
        // Loop over each element in the object
        for (auto iter = v.as_object().cbegin(); iter != v.as_object().cend(); ++iter){
            const std::string &key = iter->first;
            const web::json::value &value = iter->second;

            if (value.is_object() || value.is_array()){
                if(key.size() != 0){
                    std::wcout << "Parent: " << key.c_str() << std::endl;
                }
                printJSON(value);
                if(key.size() != 0){
                    std::wcout << "End of Parent: " << key.c_str() << std::endl;
                }
            }else{
                std::wcout << "Key: " << key.c_str() << ", Value: " << value.serialize().c_str() << std::endl;
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
            std::cout << "Got JSON reply " << v << std::endl;
            printJSON(v);
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
}
