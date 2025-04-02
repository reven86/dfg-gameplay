#include "pch.h"
#include "storefront_service.h"
#include "services/service_manager.h"




StorefrontService::StorefrontService(const ServiceManager * manager)
    : Service(manager)
{
    gameplay::Game::getInstance()->getStoreController()->getStoreFront()->setListener(this);
}

StorefrontService::~StorefrontService()
{
}

bool StorefrontService::onPreInit()
{
    return true;
}

bool StorefrontService::onTick()
{
    return false;
}

bool StorefrontService::onInit()
{
    return true;
}

bool StorefrontService::onShutdown()
{
    return true;
}

void StorefrontService::refreshProducts(const char ** productsToHandle)
{
    gameplay::Game::getInstance()->getStoreController()->getStoreFront()->getProducts(productsToHandle);
}

void StorefrontService::getProductsEvent(const std::vector< gameplay::StoreProduct >& products, const std::vector< std::string >& invalidProducts)
{
    _products = products;

#ifdef _DEBUG
    if (strcmp(gameplay::Game::getInstance()->getStoreController()->getStoreFront()->getName(), "Null"))
        for (unsigned int i = 0; i < invalidProducts.size(); i++)
            GP_WARN("Invalid product: %s", invalidProducts[i].c_str());
#endif

    _manager->signals.storefrontGetProductsSucceededEvent();
}

void StorefrontService::getProductsFailedEvent(int errorCode, const char * error)
{
    GP_WARN("Can't get products information: %d %s", errorCode, error);
    _manager->signals.storefrontGetProductsFailedEvent(errorCode, error);
}

bool StorefrontService::isProductValid(const char * product) const
{
    return getProduct(product) != nullptr;
}

const gameplay::StoreProduct * StorefrontService::getProduct(const char * product) const
{
    if (!product)
        return NULL;

    for (std::vector< gameplay::StoreProduct >::const_iterator it = _products.begin(), end_it = _products.end(); it != end_it; it++)
        if ((*it).id == product)
            return &(*it);

    return NULL;
}

void StorefrontService::paymentTransactionInProcessEvent(const char * productID, int quantity)
{
    _manager->signals.storefrontTransactionInProcessEvent(productID, quantity);
}

void StorefrontService::paymentTransactionSucceededEvent(const char * productID, int quantity, double timestamp, const char * transactionID, void * transactionObject)
{
    _manager->signals.storefrontTransactionSucceededEvent(productID, quantity, timestamp, transactionID, transactionObject);
}

void StorefrontService::paymentTransactionFailedEvent(const char * productID, int quantity, int errorCode, const char * error)
{
    GP_WARN("Transaction failed: %d", errorCode);
    _manager->signals.storefrontTransactionFailedEvent(productID, quantity, errorCode, error);
}

void StorefrontService::paymentTransactionRestoredEvent(const char * productID, int quantity, double timestamp, const char * transactionID, void * transactionObject)
{
    _manager->signals.storefrontTransactionRestoredEvent(productID, quantity, timestamp, transactionID, transactionObject);
}

void StorefrontService::receiptRequested(void * receiptData, int errorCode, const char * errorUTF8)
{
    if (errorCode != 0)
        GP_WARN("Refresh receipt failed: %d", errorCode);

    _manager->signals.storefrontReceiptRequestedEvent(receiptData, errorCode, errorUTF8);
}