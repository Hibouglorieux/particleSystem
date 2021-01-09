/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utilities.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <nallani@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/24 02:07:43 by nathan            #+#    #+#             */
/*   Updated: 2021/01/09 03:49:49 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef UTILITIES_H
# define UTILITIES_H

#include <iostream>
#include <cmath>
# define SCREEN_WIDTH 1200.0f
# define SCREEN_HEIGHT 1600.0f

void __M_Assert(const char* expr_str, bool expr, const char* file, int line, const char* msg);
void M_Assert(bool expr, const char* msg);

# ifndef NDEBUG
#   define M_Assert(Expr, Msg) \
	__M_Assert(#Expr, Expr, __FILE__, __LINE__, Msg)
#else
#   define M_Assert(Expr, Msg) ;
#endif
#  define TO_RAD(x) (x / 180.0 * M_PI)
#  define TO_DEGREE(x) (x * 180.0 / M_PI)

#endif
