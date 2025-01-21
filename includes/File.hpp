/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 17:18:38 by zouddach          #+#    #+#             */
/*   Updated: 2025/01/21 19:02:40 by zouddach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_HPP
# define FILE_HPP

# include "./Global.hpp"

class File {
	private:
	
		bool 	isValid;
		string	FileName;
		string	FilePath;
		
		ifstream file;
		
	public:
		File() : isValid(0) {};
		~File() {};
};

#endif