#pragma once
#include <unordered_map>
#include <shared_mutex>
#include <thread>

#include <pistache/http.h>
#include <pistache/http_headers.h>
#include <pistache/description.h>
#include <pistache/endpoint.h>
#include <nlohmann/json.hpp>

#include <boost/optional.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>

#include "RndData.hpp"
#include "RndGenerator.hpp"

namespace http
{

using namespace Pistache;
using json = nlohmann::json;
namespace Generic {

void handleReady(const Rest::Request&, Http::ResponseWriter response) {
    response.send(Http::Code::Ok, "1");
}

} // end namespace Generic

class DataService
{
public:
    DataService(Address addr)
        : httpEndpoint(std::make_shared<Http::Endpoint>(addr))
        , desc("Data API", "0.1")
    { }

    void init(size_t thr = 2) {
        auto opts = Http::Endpoint::options()
            .threads(static_cast<int>(thr));
        httpEndpoint->init(opts);
        createDescription();

        m_noCached.addDirective(Http::CacheDirective::NoCache);
        m_noCached.addDirective(Http::CacheDirective::NoStore);
        m_noCached.addDirective(Http::CacheDirective::MustRevalidate);
    }

    void start() {
        router.initFromDescription(desc);
        
        m_waiter = std::thread([this]()
        {
            std::printf("Press any key for stop...\n");
            std::getchar();
            httpEndpoint->shutdown();
        });

        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serveThreaded();

        m_waiter.join();
    }

private:
void createDescription() {
        desc
            .info()
            .license("Apache", "http://www.apache.org/licenses/LICENSE-2.0");

        auto backendErrorResponse =
            desc.response(Http::Code::Internal_Server_Error, "An error occured with the backend");

        desc
            .schemes(Rest::Scheme::Http)
            .basePath("/v1")
            .produces(MIME(Application, Json))
            .consumes(MIME(Application, Json));

        desc
            .route(desc.get("/ready"))
            .bind(&Generic::handleReady)
            .response(Http::Code::Ok, "Response to the /ready call")
            .hide();

        auto versionPath = desc.path("/v1");

        initRndService(versionPath);
    }

    void initRndService(Rest::Schema::SubPath& path)
    {
        auto basePath = path.path("/rnd");

        basePath
            .route(desc.post("/"))
            .bind(&DataService::createRndData, this)
            .produces(MIME(Application, Json))
            .consumes(MIME(Application, Json))
            .response(Http::Code::Ok, "Create distribution");

        basePath
            .route(desc.get("/:uuid"))
            .bind(&DataService::generateRndData, this)
            .produces(MIME(Application, Json))
            .consumes(MIME(Application, Json))
            .response(Http::Code::Ok, "Generate sample");

        basePath
            .route(desc.del("/:uuid"))
            .bind(&DataService::deleteRndData, this)
            .produces(MIME(Application, Json))
            .consumes(MIME(Application, Json))
            .response(Http::Code::Ok, "Delete distribution");
    }

    void createRndData(const Rest::Request& req, Http::ResponseWriter response) {
        auto rndDataOp = dto::rndDataFromJson(json::parse(req.body()));

        if (rndDataOp)
        {
            auto uuid = boost::lexical_cast<std::string>(boost::uuids::random_generator()());

            {
                std::unique_lock lock(m_mtxGenerators);
                m_generators.emplace(uuid, std::move(rndDataOp).value());
            }

            response.send(Http::Code::Ok, uuid);
        }
        else
        {
            response.send(Http::Code::Bad_Request, "Wrong data");
        }
    }

    void deleteRndData(const Rest::Request& req, Http::ResponseWriter response) {
        auto uuid = req.param(":uuid").as<std::string>();

        bool wasRemoved = false;
        {
            std::unique_lock lock(m_mtxGenerators);
            wasRemoved = m_generators.erase(uuid) == 1;
        }

        if (wasRemoved)
        {
            response.send(Http::Code::Ok, "Deleted");
        }
        else
        {
            response.send(Http::Code::Bad_Request, "Wrong uuid");
        }
    }

    void generateRndData(const Rest::Request& req, Http::ResponseWriter response) {
        auto uuid = req.param(":uuid").as<std::string>();

        auto generate = [this](const std::string& uuid)
        {
            std::shared_lock lock(m_mtxGenerators);
            boost::optional<json> ret;

            const auto foundIt = m_generators.find(uuid);   
            if (foundIt != std::end(m_generators))
            {
                ret = foundIt->second.generate();
            } 

            return ret;
        };

        if (auto ret = generate(uuid))
        {
            response.headers().add<Http::Header::CacheControl>(m_noCached.directives());
            response.send(Http::Code::Ok, ret.map([](auto&& el) { return el.dump(); }).get_value_or(""));
        }
        else
        {
            response.send(Http::Code::Bad_Request, "Wrong uuid");
        }
    }

private:

    std::thread m_waiter;
    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Description desc;
    Rest::Router router;

    std::unordered_map<std::string, rnd::RndGenerator> m_generators;
    std::shared_mutex m_mtxGenerators;

    Pistache::Http::Header::CacheControl m_noCached;
};

} // end namespace http