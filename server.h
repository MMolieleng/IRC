/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmoliele <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/11 09:54:42 by mmoliele          #+#    #+#             */
/*   Updated: 2017/08/11 10:23:51 by mmoliele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H

# include <stdio.h>
# include <string.h> 
# include <stdlib.h> 
# include <errno.h> 
# include <unistd.h> 
# include <arpa/inet.h> 
# include <sys/types.h> 
# include <sys/socket.h> 
# include <netinet/in.h> 
# include <sys/time.h>

#define TRUE 1
#define FALSE 0
#define PORT 8888

void	init();
#endif
