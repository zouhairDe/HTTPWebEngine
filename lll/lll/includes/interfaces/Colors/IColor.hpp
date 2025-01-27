/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IColor.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 23:58:34 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/23 00:58:33 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLOR_HPP
#define COLOR_HPP

#include <iostream>

class IColor
{
public:
    virtual ~IColor();
    virtual void applyColor(std::ostream& os) const = 0;
	
	friend std::ostream& operator<<(std::ostream& os, const IColor& color) {
        color.applyColor(os);
        return os;
    }
};

#endif
