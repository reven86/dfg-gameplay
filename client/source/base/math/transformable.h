#ifndef __DFG_TRANSFORMABLE__
#define __DFG_TRANSFORMABLE__






/*! \brief %Transformable base class for transforms in 3D space.
*	
*	\author Andrew "RevEn" Karpushin
*/

class Transformable3D
{
    gameplay::Vector3 _position;
    gameplay::Quaternion _rotation;
    gameplay::Vector3 _scale;
    gameplay::Matrix _transformation;

public:
    //! Constructs base transformable object with identity transformation matrix.
    Transformable3D ( );
    virtual ~Transformable3D ( ) { };

    //Compiler-generated copy constructor and assignment operator are fine

    //! Get position.
    const gameplay::Vector3& GetPosition ( ) const { return _position; };

    //! Get rotation.
    const gameplay::Quaternion& GetRotation( ) const { return _rotation; };

    //! Get scale.
    const gameplay::Vector3& GetScale ( ) const { return _scale; };

    //! Get transformation matrix.
    const gameplay::Matrix& GetTransformation ( ) const { return _transformation; };

    //! Set position.
    void SetPosition ( const gameplay::Vector3& pos );

    //! Set rotation.
    void SetRotation ( const gameplay::Quaternion& rot );

    //! Set scale.
    void SetScale ( const gameplay::Vector3& scale );

    //! Reset all transform: position, rotation and scale.
    void ResetTransform ( const gameplay::Vector3& pos, const gameplay::Quaternion& rot, const gameplay::Vector3& scale );
};





/*! \brief %Transformable base class for transforms in 2D space.
*	
*	\author Andrew "RevEn" Karpushin
*/

class Transformable2D
{
    gameplay::Vector2 _position;
    float _rotation;
    gameplay::Vector2 _scale;
    gameplay::Matrix3 _transformation;

public:
    //! Constructs base transformable object with identity transformation matrix.
    Transformable2D ( );
    virtual ~Transformable2D ( ) { };

    //Compiler-generated copy constructor and assignment operator are fine

    //! Get position.
    const gameplay::Vector2& GetPosition ( ) const { return _position; };

    //! Get rotation.
    const float& GetRotation ( ) const { return _rotation; };

    //! Get scale.
    const gameplay::Vector2& GetScale ( ) const { return _scale; };

    //! Get transformation matrix.
    const gameplay::Matrix3& GetTransformation ( ) const { return _transformation; };

    //! Set position.
    void SetPosition ( const gameplay::Vector2& pos );

    //! Set rotation.
    void SetRotation ( const float& rot );

    //! Set scale.
    void SetScale ( const gameplay::Vector2& scale );

    //! Reset all transform: position, rotation and scale.
    void ResetTransform ( const gameplay::Vector2& pos, const float& rot, const gameplay::Vector2& scale );
};




#endif // __DFG_TRANSFORMABLE__