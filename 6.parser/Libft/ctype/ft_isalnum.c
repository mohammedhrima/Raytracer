/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalnum.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 08:09:28 by mhrima            #+#    #+#             */
/*   Updated: 2023/08/21 08:29:06 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ctype.h"

bool	ft_isalnum(int c)
{
	return (ft_isalpha(c) || ft_isdigit(c));
}
