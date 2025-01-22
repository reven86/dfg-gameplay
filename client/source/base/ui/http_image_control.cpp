#include "pch.h"
#include "http_image_control.h"
#include "services/httprequest_service.h"
#include "services/service_manager.h"
#include "main/memory_stream.h"




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

HTTPImageControl * HTTPImageControl::create(const char* id, gameplay::Theme::Style* style)
{
    HTTPImageControl* imageControl = new HTTPImageControl();
    imageControl->_id = id ? id : "";
    imageControl->initialize(imageControl->getTypeName(), style, NULL);
    return imageControl;
}

gameplay::Control * HTTPImageControl::create(gameplay::Theme::Style* style, gameplay::Properties* properties)
{
    HTTPImageControl * control = new HTTPImageControl();
    control->initialize(control->getTypeName(), style, properties);
    return control;
}

void HTTPImageControl::initialize(const char * typeName, gameplay::Theme::Style * style, gameplay::Properties * properties)
{
    _httpRequestService = ServiceManager::getInstance()->findService<HTTPRequestService>();

    gameplay::ImageControl::initialize(typeName, style, properties);

    if (properties)
    {
        std::string path = properties->getString("path");
        setImage(path.c_str());

        _preserveAspect = properties->getBool("preserveAspect", true);
    }
}

unsigned int HTTPImageControl::drawImages(gameplay::Form * form) const
{
    return gameplay::ImageControl::drawImages(form);
}

void HTTPImageControl::setImage(const char * path)
{
    SAFE_DELETE(_batch);
    if (!path || !*path)
        return;

    if (gameplay::FileSystem::fileExists(path))
        return gameplay::ImageControl::setImage(path);

    // make sure instance is present while callback is scheduled
    this->addRef();
    _httpRequestService->makeRequestAsync({ path, "", HTTPRequestService::Request::HeadersList(),
        std::bind(&HTTPImageControl::imageDownloadedCallback, this, std::placeholders::_1, std::placeholders::_2, std::string(path), std::placeholders::_3, std::placeholders::_4) });
}

void HTTPImageControl::imageDownloadedCallback(int curlCode, MemoryStream * response, const std::string& path, const char * error, int httpResponseCode)
{
    if (curlCode != 0 || response == nullptr || httpResponseCode != 200)
    {
        this->release();
        return;
    }

    // create temporary file with an extension from original request
#ifdef WIN32
    const char * tmpFilename = tmpnam(NULL);
    std::string filename = std::string(tmpFilename) + gameplay::FileSystem::getExtension(path.c_str());
#else
    char tmpFilename[] = "tmp.XXXXXX";
    mktemp(tmpFilename);
    std::string filename = std::string(gameplay::Game::getInstance()->getTemporaryFolderPath()) + tmpFilename + gameplay::FileSystem::getExtension(path.c_str());
#endif
    std::unique_ptr<gameplay::Stream> stream(gameplay::FileSystem::open(filename.c_str(), gameplay::FileSystem::WRITE));
    if (stream && stream->write(response->getBuffer(), response->length(), 1) == 1)
    {
        stream.reset();

        gameplay::ImageControl::setImage(filename.c_str());
        if (_preserveAspect)
            setDirty(DIRTY_BOUNDS);
    }

    stream.reset();
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