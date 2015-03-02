#include "pch.h"
#include "storefront_service.h"
#include "services/service_manager.h"
#include "services/tracker_service.h"




StorefrontService::StorefrontService( const ServiceManager * manager )
    : Service( manager )
{
    gameplay::Game::getInstance( )->getStoreController( )->getStoreFront( )->setListener( this );
}

StorefrontService::~StorefrontService( )
{
}

bool StorefrontService::OnPreInit( )
{
    return true;
}

bool StorefrontService::OnTick( )
{
    return false;
}

bool StorefrontService::OnInit( )
{
    return true;
}

bool StorefrontService::OnShutdown( )
{
    return true;
}

void StorefrontService::RefreshProducts( const char ** productsToHandle )
{
    gameplay::Game::getInstance( )->getStoreController( )->getStoreFront( )->getProducts( productsToHandle );
}

void StorefrontService::getProductsEvent( const std::vector< gameplay::StoreProduct >& products, const std::vector< std::string >& invalidProducts )
{
    _products = products;
    _invalidProducts = invalidProducts;

#ifdef _DEBUG
    if( strcmp( gameplay::Game::getInstance( )->getStoreController( )->getStoreFront( )->getName( ), "Null" ) )
        for( unsigned int i = 0; i < invalidProducts.size( ); i++ )
            GP_WARN( "Invalid product: %s", invalidProducts[ i ].c_str( ) );
#endif

    _manager->signals.storefrontGetProductsSucceededEvent( );
}

void StorefrontService::getProductsFailedEvent( int errorCode, const char * error )
{
    GP_WARN( "Can't get products information: %d", errorCode );
    _manager->signals.storefrontGetProductsFailedEvent( errorCode, error );
}

bool StorefrontService::IsProductValid( const char * product ) const
{
    return std::find( _invalidProducts.begin( ), _invalidProducts.end( ), product ) == _invalidProducts.end( );
}

const gameplay::StoreProduct * StorefrontService::GetProduct( const char * product ) const
{
    for( std::vector< gameplay::StoreProduct >::const_iterator it = _products.begin( ), end_it = _products.end( ); it != end_it; it++ )
        if( ( *it ).id == product )
            return &( *it );

    return NULL;
}

void StorefrontService::paymentTransactionInProcessEvent( const char * productID, int quantity )
{
    _manager->signals.storefrontTransactionInProcessEvent( productID, quantity );
}

bool StorefrontService::paymentTransactionSucceededEvent( const char * productID, int quantity, double timestamp, const char * transactionID )
{    
    return _manager->signals.storefrontTransactionSucceededEvent( productID, quantity, timestamp, transactionID );
}

void StorefrontService::paymentTransactionFailedEvent( const char * productID, int quantity, int errorCode, const char * error )
{
    GP_WARN( "Transaction failed: %d", errorCode );
    _manager->signals.storefrontTransactionFailedEvent( productID, quantity, errorCode, error );
}

bool StorefrontService::paymentTransactionRestoredEvent( const char * productID, int quantity, double timestamp, const char * transactionID )
{
    return _manager->signals.storefrontTransactionRestoredEvent( productID, quantity, timestamp, transactionID );
}

bool StorefrontService::isProductConsumable(const char * productID)
{
    return _manager->signals.storefrontIsProductConsumable(productID);
}