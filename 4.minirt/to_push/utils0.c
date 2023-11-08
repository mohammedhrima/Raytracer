/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils0.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 00:26:37 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/09 00:26:37 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

size_t	ft_strlen(char *string)
{
	size_t	i;

	i = 0;
	while (string && string[i])
		i++;
	return (i);
}

int	ft_strcmp(char *string1, char *string2)
{
	size_t	i;

	i = 0;
	if (!string1)
		return (ft_strlen(string2));
	if (!string2)
		return (ft_strlen(string1));
	while (string1 && string2 && string1[i] && string1[i] == string2[i])
		i++;
	return (string1[i] - string2[i]);
}

void	ft_strcpy(char *destination, char *source)
{
	int	i;

	i = 0;
	while (source[i])
	{
		destination[i] = source[i];
		i++;
	}
}

char	*ft_charjoin(char *string, char c)
{
	char	*res;

	res = memory((ft_strlen(string) + 2) * sizeof(char));
	if (string)
		ft_strcpy(res, string);
	res[ft_strlen(res)] = c;
	return (res);
}

char	*ft_readline(int fd)
{
	char	*res;
	char	c;
	int		n;

	res = NULL;
	c = 0;
	while (1)
	{
		n = read(fd, &c, sizeof(char));
		if (n <= 0)
			break ;
		if (c == '\0')
			break ;
		res = ft_charjoin(res, c);
		if (c == '\n')
			break ;
	}
	return (res);
}
