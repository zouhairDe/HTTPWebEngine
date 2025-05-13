FROM ubuntu:latest

# Install nginx and clean up
RUN apt-get update && \
    apt-get install -y nginx && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Copy config (make sure the filename is correct)
COPY nginx.conf /etc/nginx/sites-available/default

# Copy webserv (put it in the same folder as Dockerfile or in a subfolder)
COPY ../* /root/webserv/

# Expose port 80
EXPOSE 80

# Start nginx in foreground
CMD ["nginx", "-g", "daemon off;"]

# 1 : docker build -t my-nginx-webserv .

# 2 : docker run -d -p 8080:80 --name webserv-container my-nginx-webserv

# 3 : cp /var/www/html/index.nginx-debian.html /var/www/html/index.html