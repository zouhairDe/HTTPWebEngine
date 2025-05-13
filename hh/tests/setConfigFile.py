import os
import re
import shutil

def parse_config(config_file):
    with open(config_file, 'r') as f:
        content = f.read()
    
    server_blocks = re.findall(r'\[server, "(.*?)"\](.*?)\["\1"\]', content, re.DOTALL)
    
    parsed_servers = []
    for server_name, block in server_blocks:
        server_config = {'name': server_name}
        
        for line in block.strip().split('\n'):
            line = line.strip()
            if line and not line.startswith('#') and '=' in line:
                key, value = [part.strip() for part in line.split('=', 1)]
                server_config[key] = value
        
        routes = re.findall(r'\[route, "(.*?)"\](.*?)\["\1"\]', block, re.DOTALL)
        server_config['routes'] = []
        for route_path, route_block in routes:
            route_config = {'path': route_path}
            for line in route_block.strip().split('\n'):
                line = line.strip()
                if line and not line.startswith('#') and '=' in line:
                    key, value = [part.strip() for part in line.split('=', 1)]
                    route_config[key] = value
            server_config['routes'].append(route_config)
        
        parsed_servers.append(server_config)
    
    return parsed_servers
    
def create_server_structure(servers):
    base_path = '/tmp/www/'
    os.makedirs(base_path, exist_ok=True)
    for server in servers:
        server_root = os.path.normpath(os.path.join(base_path, server.get('root', server['name'] + 'Root').rstrip('/')))
        os.makedirs(server_root, exist_ok=True)
        
        if 'index' in server:
            print(server)
            # Split index files by comma and process each one
            index_files = [index_file.strip() for index_file in server['index'].split(',')]
            for index_file in index_files:
                index_path = os.path.normpath(os.path.join(base_path, index_file))
                os.makedirs(os.path.dirname(index_path), exist_ok=True)
                with open(index_path, 'w') as f:
                    f.write(f'<h1>Welcome to {index_path}</h1>')
        
        # Process route-specific index files
        for route in server.get("routes", []):
            if 'index' in route:
                # Split route index files by comma and process each one
                route_index_files = [index_file.strip() for index_file in route['index'].split(',')]
                for route_index_file in route_index_files:
                    route_index_path = os.path.normpath(os.path.join(base_path, route_index_file))
                    os.makedirs(os.path.dirname(route_index_path), exist_ok=True)
                    with open(route_index_path, 'w') as f:
                        f.write(f'<h1>Welcome to {route_index_path}</h1>')
        
        if 'error_page_404' in server:
            error_dir = os.path.join(server_root, 'error')
            os.makedirs(error_dir, exist_ok=True)
            if os.path.exists('error'):
                for item in os.listdir('error'):
                    src = os.path.join('error', item)
                    dst = os.path.join(error_dir, item)
                    if os.path.isdir(src):
                        shutil.copytree(src, dst, dirs_exist_ok=True)
                    else:
                        shutil.copy2(src, dst)
        
        # Create route-specific directories
        for route in server.get('routes', []):
            if 'root' in route:
                route_root = os.path.normpath(os.path.join(base_path, route['root']))
                os.makedirs(route_root, exist_ok=True)
            
            if 'upload_store' in route:
                upload_store = os.path.normpath(os.path.join(base_path, route['upload_store']))
                os.makedirs(upload_store, exist_ok=True)

def main(config_file):
    servers = parse_config(config_file)
    create_server_structure(servers)
    print("Server structure created successfully in /tmp/www/")

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        main(sys.argv[1])
    else:
        print("Please provide a configuration file path.")