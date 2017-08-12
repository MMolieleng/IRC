/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmoliele <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/11 14:12:00 by mmoliele          #+#    #+#             */
/*   Updated: 2017/08/12 08:11:23 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

int	*init_clients()
{
	int	i;
	int	*clients;

	i = 0;
	if ((clients = (int*)malloc(sizeof(int) * MAX_CLIENTS + 1)))
	{
		while (i < MAX_CLIENTS)
		{
			clients[i] = 0;
			i++;
		}
	}
	return (clients);
}

int	init_master_socket(int opt)
{
	int	master_socket;

	if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		printf("socket failed : master");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
				sizeof(opt)) < 0)
	{
		perror("socketopt error");
		exit(EXIT_FAILURE);
	}
	return (master_socket);
}

struct sockaddr_in	init_sockaddr_in()
{
	struct sockaddr_in	addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);
	return (addr);
}

void	add_new_client(int *new_socket,int **cs)
{
	int	i;
	int	*client_socket;

	i = 0;
	client_socket = (*cs);
	while (i < MAX_CLIENTS)
	{
		if (client_socket[i] == 0)
		{
			client_socket[i] = *new_socket;
			printf("Added new client to sockets list at [%d]\n", i);
			break;
		}
		i++;
	}
}

void	check_io_operation(int **clients_list, int *sd, fd_set *readfds, int i)
{
	int	valread;
	int	*client_socket;
	char	buff[1024];

	valread = 0;
	client_socket = (*clients_list);
	while (i < MAX_CLIENTS)
	{
		*sd = client_socket[i];
		if (FD_ISSET(*sd, readfds))
		{
			if ((valread = read(*sd, buff, 1024)) == 0)
			{
				close(*sd);
				client_socket[i] = 0;
			}
		}
		else
		{
			buff[valread] = '\0';
			send(*sd, buff, strlen(buff), 0);
		}
		i++;
	}
}

int	main(int argc, char *argv[])
{
	int	opt = TRUE;
	int	master_socket;
	int	addrlen;
	int	new_socket;
	int	*client_socket;
	int	max_clients;
	int	activity;
	int	valread;
	int	sd;
	int	max_sd;
	int	i;
	struct sockaddr_in	address;
	fd_set	readfds;
	char	*message = "ECHO Daemon v1.0 \r\n";
	char	buff[1024];

	i = 0;

	client_socket = init_clients();
	master_socket = init_master_socket(opt);
	address = (init_sockaddr_in());
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		printf("Bind Error");
		exit(EXIT_FAILURE);
	}
	listen(master_socket, 3);
	addrlen = sizeof(address);
	printf("Listening to : %d ", PORT);
	while (TRUE)
	{
		FD_ZERO(&readfds);
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
		i = 0;
		while (i < MAX_CLIENTS)
		{
			sd = client_socket[i];
			if (sd > 0)
				FD_SET(sd, &readfds);
			if (sd > max_sd)
				max_sd = sd;
			i++;
		}
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
		if ((activity < 0) && (errno != EINTR))
			printf("select error");
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket, 
							(struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
			{
				printf("accept error");
				exit(EXIT_FAILURE);
			}
			printf("New connection : sock_fd = %d, IP = %s, PORT = %d\n",
					new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

			add_new_client(&new_socket, &client_socket);	
		}
		else
		check_io_operation(&client_socket, &sd, &readfds, 0);
	}
	return (0);
}
