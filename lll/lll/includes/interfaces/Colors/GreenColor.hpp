/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GreenColor.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 23:59:49 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/23 00:41:26 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef GREENCOLOR_HPP
#define GREENCOLOR_HPP

#include "IColor.hpp"

class GreenColor : public IColor
{
public:
    void applyColor(std::ostream& os) const
    {
        os << "\033[32m"; // Green text
    }
	
	virtual ~GreenColor() {}
};

#endif