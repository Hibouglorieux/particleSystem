/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/02 01:10:29 by nathan            #+#    #+#             */
/*   Updated: 2020/10/13 09:23:51 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Camera.hpp"
#include "Utilities.h"

#define X_ROTATION_SPEED 1
#define Y_ROTATION_SPEED 1

Camera::Camera() : Camera(Vec3(0, 4, 10))
{
}

Camera::Camera(float x, float y, float z) : Camera(Vec3(x, y, z))
{
}

Camera::Camera(Vec3 position)
{
	this->pos = position;
	hasTarget = false;
	dir = {0, 0, 0};
	lookAt();
	actualizeView();
}

void Camera::reset()
{
	hasTarget = true;
	dir = {0, 0, 0};
	pos = {0, 4, 10};
	lookAt();
	actualizeView();
}

Matrix Camera::getMatrix() const
{
	return view;
}

void Camera::lookAt(Vec3 target)
{
	hasTarget = true;
	dir = {0, 0, 0};
	pos = target - Vec3(0, -4, -10);
	this->target = target;
}

void Camera::freeMovement()
{
	hasTarget = false;
	dir = {0, 0, 0};
	pos = {0, 4, 10};
	actualizeView();
}

void Camera::move(bool forward, bool backward, bool right, bool left, float speedFactor)
{
	if (hasTarget)
		return;
	if (!forward && !backward && !right && !left)
		return;
	Vec3 moveDir = getDirection();
	Vec3 realMovement;
	if (forward)
		realMovement = moveDir;
	if (backward)
		realMovement -= moveDir;
	moveDir.y = 0;
	if (right)
	{
		realMovement += Matrix::createRotationMatrix(Matrix::RotationDirection::Y, 90) * moveDir;
	}
	if (left)
	{
		realMovement += Matrix::createRotationMatrix(Matrix::RotationDirection::Y, -90) * moveDir;
	}
	pos += realMovement.getNormalized() * speedFactor;
	if (realMovement != Vec3::ZERO)
	actualizeView();
}

void Camera::scroll(double factor)
{
	if (!hasTarget)
		return;
	if ((target - pos).getLength() < 5.0f && factor < 0.0f) // dont zoom too close
		return;
	Vec3 moveDir = Vec3(0, 0.1f, 1) * (float)(std::min(1.0, std::max(factor, 2.5)) * factor > 0 ? 1.0 : -1.0); // rot is applied after, need to move close to object
	pos += moveDir;
	actualizeView();
}

Vec3 Camera::getDirection() const
{
	Vec3 moveDir;
	moveDir.z = -cos(TO_RAD(dir.y)) * cos(TO_RAD(dir.x));
	moveDir.y = -sin(TO_RAD(dir.x));
	moveDir.x = sin(TO_RAD(dir.y)) * cos(TO_RAD(dir.x));
	moveDir.getNormalized();

	return moveDir;
}

void Camera::moveUp(float distance)
{
	if (!hasTarget)
	{
		pos.y += distance;
		actualizeView();
	}
}

void Camera::moveDown(float distance)
{
	if (!hasTarget)
	{
		pos.y -= distance;
		actualizeView();
	}
}

void Camera::rotate(double x, double y)
{
	if (x == 0 && y == 0)
		return;
	dir.y += x;
	dir.x += y;
	if (!hasTarget)
	{
		if (dir.x > 89.0f)
			dir.x = 89;
		if (dir.x < -89.0f)
			dir.x = -89.0f;
	}
	else
	{
		if (dir.x > 50.0f)
			dir.x = 50;
		if (dir.x < -20)
			dir.x = -20;
	}
	actualizeView();
}

void Camera::actualizeView()
{
	if (hasTarget)
	{
		rotMat = Matrix::createRotationMatrix(Matrix::RotationDirection::X, dir.x);
		rotMat *= Matrix::createRotationMatrix(Matrix::RotationDirection::Y, dir.y);
		this->view = Matrix::createTranslationMatrix(-pos) * rotMat;
	}
	else
	{
		rotMat = Matrix::createRotationMatrix(Matrix::RotationDirection::X, dir.x);
		rotMat *= Matrix::createRotationMatrix(Matrix::RotationDirection::Y, dir.y);
		this->view = rotMat * Matrix::createTranslationMatrix(-pos);
	}
}
