/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzeggaf <mzeggaf@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 17:18:38 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/05 13:27:35 by mzeggaf          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_HPP
# define FILE_HPP

# include "Global.hpp"

class File {
	private:
	
		bool 	isValid;
		string	FileName;
		string	FilePath;
		
		ifstream file;
		
	public:
		File();
		~File();
};

#endif