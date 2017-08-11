/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmoliele <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/11 09:54:50 by mmoliele          #+#    #+#             */
/*   Updated: 2017/08/11 11:27:16 by mmoliele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

int	main(int argc, char *argv[])
{
	int	opt = TRUE;
	int	master_socket;
	int	addrlen;
	int	new_socket;
	int	client_socket[30];
	int	max_clients;
	int	activity;
	int	valread;
	int	sd;
	int	max_sd;
	int	i;
	struct sockaddr_in	address;
	fd_set	readfds;
	char	*message = "ECHO Daemo v1.0 \r\n";
	char	buff[1024];

	i = 0;
	max_clients = 30;

	while (i < max_clients)
	{
		client_socket[i] = 0;
		i++;
	}

	/**
	 *Create master socket
	 */
	if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed: master");
		exit(EXIT_FAILURE);
	}

	/**
	 * set master socket to allow multiple connections,
	 * this it just a good habit, it will work without this
	 */
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
				sizeof(opt)) < 0)
	{
		perror("setsockopt error");
		exit(EXIT_FAILURE);
	}

	/**
	 * type of socket created
	 */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	/**
	 * bind the socket to localhost port 8888
	 */
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);
	/**
	 * Specify maximum of 3 pending connction for the master socket
	 */
	if (listen(master_socket, 3) < 0)
	{
		perror("Listen");
		exit(EXIT_FAILURE);
	}

	/**
	 * accept incomming connections
	 */
	addrlen = sizeof(address);
	printf("Waiting for connections\n");
	while (TRUE)
	{
		/**
		 * clear the sockt set
		 */
		FD_ZERO(&readfds);

		/**
		 * add master socket to set
		 */
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		/**
		 * add child sockets to set
		 */
		i = 0;
		while (i < max_clients)
		{
			sd = client_socket[i];

			/**
			 * if valid socket descriptor then add to read list
			 */
			if (sd > 0)
				FD_SET(sd, &readfds);

			/**
			 * highest file descriptor number,
			 * nedd it for the select function
			 */
			if (sd > max_sd)
				max_sd = sd;
			i++;
		}

		/**
		 * wait for an activity on one of the sockets, timeout is NULL
		 * so wair indefinitely
		 */
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
		if ((activity < 0) && (errno != EINTR))
			printf("select error");

		/**
		 * if something happend on the master socket
		 * then it's an incomming connection
		 */
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket, 
					(struct sockaddr *)&address,(socklen_t*)&addrlen)) < 0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			/**
			 * inform user of socket number
			 * used in send and receive commands
			 */
			printf("New connection, socket fd is %d, IP is : %s ,Port is: %d\n",
			 new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

			if (send(new_socket, message, strlen(message), 0) != strlen(message))
				perror("Send Success");
			printf("Welcome message send successfully\n");

			/**
			 * add new socket to array of sockets
			 */
			i = 0;
			while (i < max_clients)
			{
				if (client_socket[i] == 0)
				{
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n", i);
					break;
				}
				i++;
			}
		}

		/**
		 * Else it's some IO operation on some other socket
		 */
		i = 0;
		while (i < max_clients)
		{
			sd = client_socket[i];
			if (FD_ISSET(sd, &readfds))
			{
				/**
				 * Check if it was for closing, and also read the
				 * incoming message
				 */
				if ((valread = read(sd, buff, 1024)) == 0)
				{
					/**
					 * Somebody is disconnected, get their details and print
					 */
					getpeername(sd, (struct sockaddr*)&address,
							(socklen_t*)&addrlen);
					printf("Host disconnected, IP %s, PORT %d\n",
						inet_ntoa(address.sin_addr), ntohs(address.sin_port));
					/**
					 * Close the socket and mark as 0 in the list for reuse
					 */
					close(sd);
					client_socket[i] = 0;
				}
				else
				{
					/**
					 * Echo back what has been send
					 */
					buff[valread] = '\0';
					send(sd, buff, strlen(buff), 0);
				}
			}
			i++;
		}
	}
	return (0);
}
