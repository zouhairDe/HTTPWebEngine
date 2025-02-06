/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzeggaf <mzeggaf@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 17:18:38 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/06 13:46:45 by mzeggaf          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_HPP
# define FILE_HPP

# include "Global.hpp"

class File {
	private:
		string		_path;
		size_t		_size;
		char		*_data;
		
	public:
		File();
		File(string path);
		~File();

		/* getters */
		size_t		getSize() const;
		char		*getData() const;
		string		getPath() const;
		bool		exists() const;
};

#endif