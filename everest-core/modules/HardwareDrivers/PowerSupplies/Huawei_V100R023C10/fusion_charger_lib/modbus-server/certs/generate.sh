openssl genrsa -out ca.key.pem 2048
openssl genrsa -out server.key.pem 2048
openssl genrsa -out client.key.pem 2048
openssl req -new -x509 -days 1000 -key ca.key.pem -out ca.crt.pem -subj "/C=DE/O=Frickly Systems GmbH/CN=The one and only Root CA"

openssl req -new -key server.key.pem -out server.csr.pem -subj "/C=DE/O=Frickly Systems GmbH/CN=localhost"
openssl req -new -key client.key.pem -out client.csr.pem -subj "/C=DE/O=Frickly Systems GmbH/CN=client"

openssl x509 -req -in server.csr.pem -out server.crt.pem -CA ca.crt.pem -CAkey ca.key.pem -CAcreateserial -days 1000
openssl x509 -req -in client.csr.pem -out client.crt.pem -CA ca.crt.pem -CAkey ca.key.pem -CAcreateserial -days 1000
