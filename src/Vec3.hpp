/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vec3.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/26 16:12:34 by nathan            #+#    #+#             */
/*   Updated: 2022/05/27 21:16:01 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef VEC3_CLASS_H
# define VEC3_CLASS_H

#include <sstream>
#include <string>
#include <cmath>
#include <iostream>

class Vec3 {
public:
	static const Vec3 ZERO;
	static const Vec3 ONE;
	Vec3( void );
	Vec3( float xx, float yy, float zz );
	Vec3( const Vec3& copy );
	~Vec3(){}

	float getSquaredLength() const;
	float getLength() const;
	Vec3 getAngle() const;
	Vec3 getNormalized() const;
	float dot(Vec3 rhs) const;
	Vec3 cross(Vec3 rhs) const;
	std::string toString() const;
	void print() const;
	
	Vec3 operator+( const Vec3& rhs ) const;
	Vec3 operator+=( const Vec3& rhs);
	Vec3 operator-( const Vec3& rhs ) const;
	Vec3 operator-=( const Vec3& rhs);
	Vec3 operator*( const float scale ) const;
	Vec3 operator*( const Vec3& rhs ) const;
	Vec3 operator/=( const float value ) const;
	bool operator==( const Vec3& rhs ) const;
	bool operator!=( const Vec3& rhs ) const;
	Vec3& operator=( const Vec3& rhs );
	Vec3 operator-() const;
	float at(const int i) const;
	float& operator[]( const int i );
	friend std::ostream& operator<<(std::ostream& os, const Vec3& vec);

	float x, y, z;
private:
};

#endif
