/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultColor.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 00:00:56 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/23 00:41:23 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFAULTCOLOR_HPP
#define DEFAULTCOLOR_HPP

#include "IColor.hpp"

class DefaultColor : public IColor
{
public:
    void applyColor(std::ostream& os) const
    {
        os << "\033[39m"; // Default text color
    }
	
	virtual ~DefaultColor() {}
};

#endif