openssl genrsa -out psu_ca.key.pem 2048
openssl genrsa -out dispenser_ca.key.pem 2048

openssl genrsa -out psu.key.pem 2048
openssl genrsa -out dispenser.key.pem 2048

openssl req -new -x509 -days 1000 -key psu_ca.key.pem -out psu_ca.crt.pem -subj "/C=DE/O=Frickly Systems GmbH/CN=The one and only Root CA"
openssl req -new -x509 -days 1000 -key dispenser_ca.key.pem -out dispenser_ca.crt.pem -subj "/C=DE/O=Frickly Systems GmbH/CN=The one and only Root CA"

openssl req -new -key psu.key.pem -out psu.csr.pem -subj "/C=DE/O=Frickly Systems GmbH/CN=localhost"
openssl req -new -key dispenser.key.pem -out dispenser.csr.pem -subj "/C=DE/O=Frickly Systems GmbH/CN=client"

openssl x509 -req -in psu.csr.pem -out psu.crt.pem -CA psu_ca.crt.pem -CAkey psu_ca.key.pem -CAcreateserial -days 1000
openssl x509 -req -in dispenser.csr.pem -out dispenser.crt.pem -CA dispenser_ca.crt.pem -CAkey dispenser_ca.key.pem -CAcreateserial -days 1000
