/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 00:26:57 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/09 00:42:03 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void	*ft_memset(void *pointer, int c, size_t len)
{
	size_t			i;
	unsigned char	*destionation;

	destionation = (unsigned char *)pointer;
	i = 0;
	while (i < len)
	{
		*destionation = (unsigned char)c;
		destionation++;
		i++;
	}
	return (pointer);
}

uintptr_t	*add_to_backup(uintptr_t *backup, void *array, int *pos_ptr,
		int *len_ptr)
{
	int		pos;
	void	*tmp;
	int		len;

	pos = *pos_ptr;
	len = *len_ptr;
	backup[pos++] = (uintptr_t)array;
	if (pos + 3 == len)
	{
		tmp = malloc(len * 2 * sizeof(uintptr_t));
		ft_memcpy(tmp, backup, len * sizeof(uintptr_t));
		len *= 2;
		free(backup);
		backup = tmp;
	}
	*pos_ptr = pos;
	*len_ptr = len;
	return (backup);
}

void	*allocate(size_t size, uintptr_t **backup, int *pos, int *len)
{
	void	*array;

	array = (void *)malloc(size);
	if (!array)
	{
		printf("Error allocation in calloc\n");
		exit(1);
	}
	ft_memset(array, 0, size);
	*backup = add_to_backup(*backup, array, pos, len);
	return (array);
}

void	*memory(size_t size)
{
	static uintptr_t	*backup;
	static int			pos;
	static int			len;
	int					i;

	if (size > 0)
	{
		if (backup == NULL)
		{
			len = 100;
			backup = malloc(len * sizeof(uintptr_t));
		}
		return (allocate(size, &backup, &pos, &len));
	}
	printf("freeing memory\n");
	exit(0);
	i = 0;
	while (i < pos)
		free((void *)(backup[i++]));
	free(backup);
	return (NULL);
}
