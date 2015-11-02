#ifndef __DFG_STOREFRONTSERVICE__
#define __DFG_STOREFRONTSERVICE__

#include "service.h"




/** @brief Storefront service controls IAPs.
 */

class StorefrontService : public Service, public gameplay::StoreListener
{
    friend class ServiceManager;

public:
    static const char * getTypeName() { return "StorefrontService"; }

    void refreshProducts(const char ** productsToHandle);

    /**
     * Check whether the product is valid or not.
     *
     * Invalid products should not be displayed in UI since they indicate
     * an error in product's configuration in the Store.
     */
    bool isProductValid(const char * product) const;

    /**
     * Get product by its id.
     *
     * Return NULL if product information is not available (e.g. no connection).
     * Call to RefreshProducts invalidates all previously returned products.
     */
    const gameplay::StoreProduct * getProduct(const char * product) const;

protected:
    StorefrontService(const ServiceManager * manager);
    virtual ~StorefrontService();

    virtual bool onPreInit();
    virtual bool onInit();
    virtual bool onTick();
    virtual bool onShutdown();

    /**
     * @see gameplay::StoreListener::getProductsEvent
     */
    virtual void getProductsEvent(const std::vector< gameplay::StoreProduct >& products, const std::vector< std::string >& invalidProducts);

    /**
     * @see gameplay::StoreListener::getProductsFailedEvent
     */
    virtual void getProductsFailedEvent(int errorCode, const char * error);

    /**
     * @see gameplay::StoreListener::paymentTransactionInProcessEvent
     */
    virtual void paymentTransactionInProcessEvent(const char * productID, int quantity);

    /**
     * @see gameplay::StoreListener::paymentTransactionSucceededEvent
     */
    virtual void paymentTransactionSucceededEvent(const char * productID, int quantity, double timestamp, const char * transactionID, void * transactionObject);

    /**
     * @see gameplay::StoreListener::paymentTransactionFailedEvent
     */
    virtual void paymentTransactionFailedEvent(const char * productID, int quantity, int errorCode, const char * error);

    /**
     * @see gameplay::StoreListener::paymentTransactionRestoredEvent
     */
    virtual void paymentTransactionRestoredEvent(const char * productID, int quantity, double timestamp, const char * transactionID, void * transactionObject);

    /**
    * @see gameplay::StoreListener::isProductConsumable
    */
    virtual bool isProductConsumable(const char * productID);

private:
    std::vector< gameplay::StoreProduct > _products;
    std::vector< std::string > _invalidProducts;
};



#endif //__DFG_STOREFRONTSERVICE__