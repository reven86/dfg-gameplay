#include "pch.h"
#include "http_image_control.h"
#include "services/httprequest_service.h"
#include "services/service_manager.h"




HTTPImageControl::HTTPImageControl()
    : _httpRequestService(NULL)
{
}

HTTPImageControl::~HTTPImageControl()
{
}

const char * HTTPImageControl::getTypeName() const
{
    return "HTTPImageControl";
}

gameplay::Control * HTTPImageControl::create(gameplay::Theme::Style* style, gameplay::Properties* properties)
{
    HTTPImageControl * control = new HTTPImageControl();
    control->_httpRequestService = ServiceManager::getInstance()->findService<HTTPRequestService>();
    control->initialize(control->getTypeName(), style, properties);
    return control;
}

void HTTPImageControl::initialize(const char * typeName, gameplay::Theme::Style * style, gameplay::Properties * properties)
{
    gameplay::ImageControl::initialize(typeName, style, properties);

    if (properties)
    {
        std::string path = properties->getString("path");
        if (!gameplay::FileSystem::fileExists(path.c_str()))
        {
            setImage(path.c_str());
        }
    }
}

unsigned int HTTPImageControl::drawImages(gameplay::Form * form) const
{
    return gameplay::ImageControl::drawImages(form);
}

void HTTPImageControl::setImage(const char * path)
{
    if (gameplay::FileSystem::fileExists(path))
        return gameplay::ImageControl::setImage(path);

    _httpRequestService->makeRequestAsync(path, NULL, std::bind(&HTTPImageControl::imageDownloadedCallback, this, std::placeholders::_1, std::placeholders::_2, std::string(path)));
}

void HTTPImageControl::imageDownloadedCallback(int curlCode, const std::vector<uint8_t>& response, const std::string& path)
{
    if (curlCode != 0)
        return;

    // create temporary file with an extension from original request
    std::string filename = std::string(gameplay::Game::getInstance()->getTemporaryFolderPath()) + tmpnam(NULL) + gameplay::FileSystem::getExtension(path.c_str());
    std::unique_ptr<gameplay::Stream> stream(gameplay::FileSystem::open(filename.c_str(), gameplay::FileSystem::WRITE));
    stream->write(&response.front(), response.size(), 1);
    stream.reset();

    gameplay::ImageControl::setImage(filename.c_str());
}