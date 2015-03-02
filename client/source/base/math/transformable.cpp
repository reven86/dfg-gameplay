#include "pch.h"
#include "transformable.h"





//
// Transformable3D
//

Transformable3D::Transformable3D( )
    : _position( gameplay::Vector3::zero( ) )
    , _rotation( gameplay::Quaternion::identity( ) )
    , _scale( gameplay::Vector3::one( ) )
    , _transformation( gameplay::Matrix::identity( ) )
{
}

void Transformable3D::SetPosition( const gameplay::Vector3& pos )
{
    _position = pos;
    _transformation.m[12] = pos.x;
    _transformation.m[13] = pos.y;
    _transformation.m[14] = pos.z;
}

void Transformable3D::SetRotation( const gameplay::Quaternion& rot )
{
    _rotation = rot;
    gameplay::Matrix::createTranslation( _position, &_transformation );
    _transformation.rotate( _rotation );
    _transformation.scale( _scale );
}

void Transformable3D::SetScale( const gameplay::Vector3& scale )
{
    _scale = scale;
    gameplay::Matrix::createTranslation( _position, &_transformation );
    _transformation.rotate( _rotation );
    _transformation.scale( _scale );
}

void Transformable3D::ResetTransform( const gameplay::Vector3& pos, const gameplay::Quaternion& rot, const gameplay::Vector3& scale )
{
    _position	= pos;
    _rotation	= rot;
    _scale		= scale;

    gameplay::Matrix::createTranslation( _position, &_transformation );
    _transformation.rotate( _rotation );
    _transformation.scale( _scale );
}





//
// Transformable2D
//

Transformable2D::Transformable2D( )
    : _position( gameplay::Vector2::zero( ) )
    , _rotation( 0.0f )
    , _scale( gameplay::Vector2::one( ) )
    , _transformation( gameplay::Matrix3::identity( ) )
{
}

void Transformable2D::SetPosition( const gameplay::Vector2& pos )
{
    _position = pos;

    _transformation.m[6] = pos.x;
    _transformation.m[7] = pos.y;
}

void Transformable2D::SetRotation( const float& rot )
{
    _rotation = rot;

    gameplay::Matrix3::createTranslation( _position, &_transformation );
    _transformation.rotate( _rotation );
    _transformation.scale( _scale );
}

void Transformable2D::SetScale( const gameplay::Vector2& scale )
{
    _scale = scale;

    gameplay::Matrix3::createTranslation( _position, &_transformation );
    _transformation.rotate( _rotation );
    _transformation.scale( _scale );
}

void Transformable2D::ResetTransform( const gameplay::Vector2& pos, const float& rot, const gameplay::Vector2& scale )
{
    _position	= pos;
    _rotation	= rot;
    _scale		= scale;

    gameplay::Matrix3::createTranslation( _position, &_transformation );
    _transformation.rotate( _rotation );
    _transformation.scale( _scale );
}