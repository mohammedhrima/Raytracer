/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 00:42:55 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/09 00:42:55 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

char	*ft_strchr(char *string, int c)
{
	int	i;

	i = 0;
	if (!string)
		return (NULL);
	while (string[i])
	{
		if (string[i] == (char)c)
			return (string + i);
		i++;
	}
	if (c == 0 && string[i] == 0)
		return (string + i);
	return (NULL);
}

bool	ft_isspace(int c)
{
	return (ft_strchr("\t\n\v\f\r ", c));
}

char	*ft_strstr(char *from, char *to_find)
{
	int	i;
	int	k;
	int	j;

	if (!from || !to_find)
		return (NULL);
	i = 0;
	while (from[i])
	{
		k = i;
		j = 0;
		while (from[k] == to_find[j] && to_find[j] && from[k])
		{
			k++;
			j++;
		}
		if (!to_find[j])
			return (from + i);
		i++;
	}
	return (NULL);
}

bool	ft_isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

void	*ft_memcpy(void *destination, void *source, size_t len)
{
	size_t	i;
	char	*pointer1;
	char	*pointer2;

	if (!destination)
		return (source);
	if (!source)
		return (destination);
	pointer1 = (char *)destination;
	pointer2 = (char *)source;
	i = 0;
	while (i < len)
	{
		pointer1[i] = pointer2[i];
		i++;
	}
	return (destination);
}
