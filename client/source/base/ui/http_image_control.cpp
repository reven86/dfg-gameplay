#include "pch.h"
#include "http_image_control.h"
#include "services/httprequest_service.h"
#include "services/service_manager.h"




HTTPImageControl::HTTPImageControl()
    : _httpRequestService(NULL)
    , _preserveAspect(true)
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

        _preserveAspect = properties->getBool("preserveAspect", true);
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

    // make sure instance is present while callback is scheduled
    this->addRef();
    _httpRequestService->makeRequestAsync(path, NULL, std::bind(&HTTPImageControl::imageDownloadedCallback, this, std::placeholders::_1, std::placeholders::_2, std::string(path)));
}

void HTTPImageControl::imageDownloadedCallback(int curlCode, const std::vector<uint8_t>& response, const std::string& path)
{
    if (curlCode != 0)
    {
        this->release();
        return;
    }

    // create temporary file with an extension from original request
#ifdef WIN32
    const char * tmpFilename = tmpname();
#else
    char tmpFilename[] = "tmp.XXXXX";
    mktemp(tmpFilename);
#endif
    std::string filename = std::string(gameplay::Game::getInstance()->getTemporaryFolderPath()) + tmpFilename + gameplay::FileSystem::getExtension(path.c_str());
    std::unique_ptr<gameplay::Stream> stream(gameplay::FileSystem::open(filename.c_str(), gameplay::FileSystem::WRITE));
    if (stream)
        stream->write(&response.front(), response.size(), 1);
    stream.reset();

    gameplay::ImageControl::setImage(filename.c_str());
    if (_preserveAspect)
        setDirty(DIRTY_BOUNDS);

    remove(filename.c_str());

    this->release();
}

void HTTPImageControl::setPreserveAspect(bool set)
{
    _preserveAspect = set;
    setDirty(DIRTY_BOUNDS);
}

void HTTPImageControl::updateBounds()
{
    gameplay::ImageControl::updateBounds();

    if (_preserveAspect && _tw > 0.0f && _th > 0.0f)
    {
        if (isWidthPercentage() && !isHeightPercentage())
            setHeight(_bounds.width * _tw / _th);
        else if (!isWidthPercentage() && isHeightPercentage())
            setWidth(_bounds.height * _th / _tw);
    }
}