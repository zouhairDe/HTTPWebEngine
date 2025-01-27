/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BoldFont.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 01:00:17 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/23 01:00:48 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOLDFONT_HPP
#define BOLDFONT_HPP

#include "IColor.hpp"

class BoldFont : public IColor
{
public:
	void applyColor(std::ostream& os) const
	{
		os << "\033[1m"; // Bold text
	}
	
	virtual ~BoldFont() {}
};

#endif