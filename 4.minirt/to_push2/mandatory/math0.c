/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   math0.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhrima <mhrima@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 22:04:02 by mhrima            #+#    #+#             */
/*   Updated: 2023/11/08 22:08:18 by mhrima           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

t_vec3	unit_vector(t_vec3 v)
{
	float	f;

	f = length(v);
	if (f <= ZERO)
		return ((t_vec3){});
	return (scale(v, '/', f));
}

float	dot(t_vec3 u, t_vec3 v)
{
	return (u.x * v.x + u.y * v.y + u.z * v.z);
}
