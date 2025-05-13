/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RedColor.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 23:59:23 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/23 00:41:28 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef REDCOLOR_HPP
#define REDCOLOR_HPP

#include "IColor.hpp"

class RedColor : public IColor
{
public:
    void applyColor(std::ostream& os) const
    {
        os << "\033[31m"; // Red text
    }
	
	virtual ~RedColor() {}
};

#endif
