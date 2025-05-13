/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlueColor.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 00:00:16 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/23 00:41:17 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef BLUECOLOR_HPP
#define BLUECOLOR_HPP

#include "IColor.hpp"

class BlueColor : public IColor
{
public:
    void applyColor(std::ostream& os) const
    {
        os << "\033[34m"; // Blue text
    }
	
	virtual ~BlueColor() {}
};

#endif
