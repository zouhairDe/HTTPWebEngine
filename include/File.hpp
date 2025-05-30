/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zouddach <zouddach@1337.student.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 17:18:38 by zouddach          #+#    #+#             */
/*   Updated: 2025/02/25 22:58:45 by zouddach         ###   ########.fr       */
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
		File(const File &other);
		File	&operator=(const File &other);
		~File();

		/* getters */
		size_t		getSize() const;
		char		*getData() const;
		string		getPath() const;
		bool		exists() const;
		void		setData(string Data, size_t size);

};

ostream &operator<<(ostream &os, const File &file);

#endif