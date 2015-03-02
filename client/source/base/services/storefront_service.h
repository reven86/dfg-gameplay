#ifndef __DFG_STOREFRONTSERVICE__
#define __DFG_STOREFRONTSERVICE__

#include "service.h"




/*! \brief Storefront service that controls IAPs.
 */

class StorefrontService : public Service, public gameplay::StoreListener
{
    std::vector< gameplay::StoreProduct > _products;
    std::vector< std::string > _invalidProducts;

public:
	StorefrontService ( const ServiceManager * manager );
	virtual ~StorefrontService ( );
    
	static const char * GetName( ) { return "StorefrontService"; }
    
    virtual bool OnPreInit( );
	virtual bool OnInit( );
    virtual bool OnTick( );
	virtual bool OnShutdown( );

    void RefreshProducts( const char ** productsToHandle );

    /*! Check whether the product is valid or not.
     *
     *  Invalid products should not be displayed in UI since they indicate
     *  an error in product's configuration in the Store.
     */
    bool IsProductValid( const char * product ) const;

    /*! Get product by its id.
     *
     *  Return NULL if product information is not available (e.g. no connection).
     *  Call to RefreshProducts invalidates all previously returned products.
     */
    const gameplay::StoreProduct * GetProduct( const char * product ) const;
    
protected:
    /**
     * @see gameplay::StoreListener::getProductsEvent
     */
    virtual void getProductsEvent( const std::vector< gameplay::StoreProduct >& products, const std::vector< std::string >& invalidProducts );

    /**
     * @see gameplay::StoreListener::getProductsFailedEvent
     */
    virtual void getProductsFailedEvent( int errorCode, const char * error );

    /**
     * @see gameplay::StoreListener::paymentTransactionInProcessEvent
     */
    virtual void paymentTransactionInProcessEvent( const char * productID, int quantity );

    /**
     * @see gameplay::StoreListener::paymentTransactionSucceededEvent
     */
    virtual bool paymentTransactionSucceededEvent( const char * productID, int quantity, double timestamp, const char * transactionID );
    
    /**
     * @see gameplay::StoreListener::paymentTransactionFailedEvent
     */
    virtual void paymentTransactionFailedEvent( const char * productID, int quantity, int errorCode, const char * error );
    
    /**
     * @see gameplay::StoreListener::paymentTransactionRestoredEvent
     */
    virtual bool paymentTransactionRestoredEvent( const char * productID, int quantity, double timestamp, const char * transactionID );

    /**
    * @see gameplay::StoreListener::isProductConsumable
    */
    virtual bool isProductConsumable(const char * productID);
};



#endif //__DFG_STOREFRONTSERVICE__