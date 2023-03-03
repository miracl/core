/*
 * Copyright (c) 2012-2020 MIRACL UK Ltd.
 *
 * This file is part of MIRACL Core
 * (see https://github.com/miracl/core).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

 /* This simple driver program is hard-wired to support just one elliptic curve and one
   RSA bit length. To change replace the text string NIST256 in the main program to
   another curve and 2048 to some other value, where the curve and the RSA bit length
   are suppported by the library. Of course a more elaborate program could support
   multiple curves simultaneously */

extern crate mcore;

use mcore::x509;
use mcore::hmac;

// USE_NIST256
const CHOICE: usize=x509::USE_NIST256;
use mcore::nist256::ecdh;
use mcore::rsa2048::rsa;
// RSA 2048 Self-Signed CA cert
const CA_B64: &str="MIIDuzCCAqOgAwIBAgIJAP44jcM1MOROMA0GCSqGSIb3DQEBCwUAMHQxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdJcmVsYW5kMQ8wDQYDVQQHDAZEdWJsaW4xITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDEfMB0GCSqGSIb3DQEJARYQbXNjb3R0QGluZGlnby5pZTAeFw0xNTExMjYwOTUwMzlaFw0yMDExMjUwOTUwMzlaMHQxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdJcmVsYW5kMQ8wDQYDVQQHDAZEdWJsaW4xITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDEfMB0GCSqGSIb3DQEJARYQbXNjb3R0QGluZGlnby5pZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANUs7/nri9J8zw8rW8JVszXP0ZqeLoQJaq2X28ebm8x5VT3okr9rnBjFjpx0YKQCAFQf8iSOOYuNpDvtZ/YpsjPbk2rg5sLY9G0eUMqrTuZ7moPSxnrXS5evizjD9Z9HqaqeNEYD3sPouPg+lhU1oAUQjUTJVFhEr1x0EnSEYbbrWtY9ZDSuZv+d4NIeqqPOYFd1yZc+LYZyQbAAQqwRLNPZH/rnIykLa6I7w7mGT7H6SBz2O09BtgpTHhalL40ecXa4ZOEze0xwzlc+mEFIrnmdadg3vQrJt42RVbo3LN6RfDIqUZOMOtQW/53pUR1lIpCwVWJTiOpmSEIEqhhjFq0CAwEAAaNQME4wHQYDVR0OBBYEFJrz6LHeT6FcjRahpUC3hAMxKRTCMB8GA1UdIwQYMBaAFJrz6LHeT6FcjRahpUC3hAMxKRTCMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBADqkqCYVa3X8XO9Ufu6XIUoZafFPRjSeJXvEIWqlbm7ixJZ2FPOvf2eMc5RCZYigNKhsxru5Ojw0lPcpa8DDmEsdZDf7p0vlmf7T7xH9gtoInh4DzgI8HRHFc8R/z2/jLX7nlLoopKX5yp7F1gRACg0pd4tGpQ6EnBNcYZZghFH9UIRDmx+vDlwDCu8vyRPt35orrEiI4XGq/QkvxxAb5YWxQ4i06064ULfyCI7suu3KoobdM1aAaA8zhpOOBXKbq+Wi9IGFe/wiEMHLmfHdt9CBTjIWb//IHji4RT05kCmTVrx97pb7EHafuL3L10mM5cpTyBWKnb4kMFtx9yw+S2U=";
// an RSA 2048 CA-signed cert
//const CERT_B64: &str="MIIDcjCCAloCAQEwDQYJKoZIhvcNAQELBQAwdDELMAkGA1UEBhMCSUUxEDAOBgNVBAgMB0lyZWxhbmQxDzANBgNVBAcMBkR1YmxpbjEhMB8GA1UECgwYSW50ZXJuZXQgV2lkZ2l0cyBQdHkgTHRkMR8wHQYJKoZIhvcNAQkBFhBtc2NvdHRAaW5kaWdvLmllMB4XDTE1MTEyNjEwMzQzMFoXDTE3MTEyNTEwMzQzMFowgYkxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdJcmVsYW5kMQ8wDQYDVQQHDAZEdWJsaW4xETAPBgNVBAoMCENlcnRpVm94MQ0wCwYDVQQLDARMYWJzMQ0wCwYDVQQDDARNSUtFMSYwJAYJKoZIhvcNAQkBFhdtaWtlLnNjb3R0QGNlcnRpdm94LmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMIoxaQHFQzfyNChrw+3i7FjRFMHZ4zspkjkAcJW21LdBCqrxU+sdjyBoSFlrlafQOHshbrEP93AKX1bfaYbuV4fzq7OlRaLxaK+b+xrOJdewMI2WZ5OwEzj3onZATISogIoB6dTdzJ41NuxuMqQ/DqOnVrRA0SoIespbQhB8FGHBLw0hJATBzUk+bqOIt0HmnMp2EbYgtuG4lYINU/lD3Qt16SunUukWRLtxqJkioie+dkhP2zm+bOlSVmeQb4Wp8AI14OKkTfkdYC8qCxb5eabg90Q33rQUhNwRQHhHwopZwD/BgodasoSrPfwUlj0awh6y87eMGcik5Q/mjkCk5MCAwEAATANBgkqhkiG9w0BAQsFAAOCAQEAFrd7R/67ClkbLhpiX++6QTOa47siUAB9v+Qil9hZfhPNeeM589ixYkD4zH5pOK2B0ea+CXEKkanQ6lXx9KV86yS7fq6Yww7wO0diecusHd0+P82i46Tq0nm8nlsnAuhYoFRUGa2m2DkB1HSsB0ts8DjzFLySonFjSSLHDU0ox9/uFbJMzipy3ijAA4XM0N4jRrUfrmxpA7DOOsbEbGkvvB7VK9+s9PHE/4dJTwhSteplUnhxVFkkDo/JwaLx4/IEQRlCF3KEQ5s3AwRHnbrIjOY2yONxHBtJEp7QN5aOHruwvMNRNheCBPiQJyLitUsFGr4voANmobkrFgYtu0tRMQ==";
// an ECC 256 CA-signed cert
const CERT_B64: &str = "MIICojCCAYoCAQMwDQYJKoZIhvcNAQELBQAwdDELMAkGA1UEBhMCSUUxEDAOBgNVBAgMB0lyZWxhbmQxDzANBgNVBAcMBkR1YmxpbjEhMB8GA1UECgwYSW50ZXJuZXQgV2lkZ2l0cyBQdHkgTHRkMR8wHQYJKoZIhvcNAQkBFhBtc2NvdHRAaW5kaWdvLmllMB4XDTE1MTEyNjEzNDcyOVoXDTE3MTEyNTEzNDcyOVowgYQxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdJcmVsYW5kMQ8wDQYDVQQHDAZEdWJsaW4xETAPBgNVBAoMCENlcnRpdm94MQ0wCwYDVQQLDARMYWJzMQ8wDQYDVQQDDAZtc2NvdHQxHzAdBgkqhkiG9w0BCQEWEG1zY290dEBpbmRpZ28uaWUwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAATO2iZiQZsXxzwBKnufKfZcsctNXZ4PmfJm638PmX9DQ3Xdb+nD5VxiOakNcB9xf5im8CriiOF5Z/7yPGyzUMbdMA0GCSqGSIb3DQEBCwUAA4IBAQAK5fMgGCCiPts8hMUZvYDpu8hd7qtPKPBc10QUccHb7PGrhqf/Ex2Gpj1aaURmx7SGZG0HX97LtkdW8KQpEoyaa60r7cjVA589TznxXKSGg5ggVoFJNpuZUm7VcolLjwIgTxtGbPzrvVMiZ4cl4PwFePXVKTl4f8XkOFX5gLmVSuCf729lEBmpx3IzqGmTjmnBixaApUElOKVeL7hiUKP3TqMUxZN+QNJBq4Mh9K9h4Sks2oneLwBwhMqQvpmcOb/7SucJn5N0IgJoGaMbfX0oCJJID1NSbagUSbFD1XciR2Ng9VtvnRP+htmEQ7jtww8phFdrWt5M5zPGOHUppqDx";
// ** CA is ECC 256 based  - for use with NIST256 build of library
// ECC 256 Self-Signed CA cert
//const CA_B64: &str="MIIB7TCCAZOgAwIBAgIJANp4nGS/VYj2MAoGCCqGSM49BAMCMFMxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdJcmVsYW5kMQ8wDQYDVQQHDAZEdWJsaW4xITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDAeFw0xNTExMjYxMzI0MTBaFw0yMDExMjUxMzI0MTBaMFMxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdJcmVsYW5kMQ8wDQYDVQQHDAZEdWJsaW4xITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABPb6IjYNKyfbEtL1aafzW1jrn6ALn3PnGm7AyX+pcvwG0GKmb3Z/uHzhT4GysNE0/GB1n4Y/mrORQIm2X98rRs6jUDBOMB0GA1UdDgQWBBSfXUNkgJVklIhuXq4DCnVYhsdzwDAfBgNVHSMEGDAWgBSfXUNkgJVklIhuXq4DCnVYhsdzwDAMBgNVHRMEBTADAQH/MAoGCCqGSM49BAMCA0gAMEUCIQDrZJ1tshwTl/jabU2i49EOgbWe0ZgE3QZywJclf5IVwwIgVmz79AAf7e098lyrOKYAqbwjHVyMZGfmkNNGIuIhp/Q=";
// an ECC 256 CA-signed cert
//const CERT_B64: &str = "MIIBvjCCAWQCAQEwCgYIKoZIzj0EAwIwUzELMAkGA1UEBhMCSUUxEDAOBgNVBAgMB0lyZWxhbmQxDzANBgNVBAcMBkR1YmxpbjEhMB8GA1UECgwYSW50ZXJuZXQgV2lkZ2l0cyBQdHkgTHRkMB4XDTE1MTEyNjEzMjc1N1oXDTE3MTEyNTEzMjc1N1owgYIxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdJcmVsYW5kMQ8wDQYDVQQHDAZEdWJsaW4xETAPBgNVBAoMCENlcnRpdm94MQ0wCwYDVQQLDARMYWJzMQ0wCwYDVQQDDARtaWtlMR8wHQYJKoZIhvcNAQkBFhBtc2NvdHRAaW5kaWdvLmllMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEY42H52TfWMLueKB1o2Sq8uKaKErbHJ2GRAxrnJdNxex0hxZF5FUx7664BbPUolKhpvKTnJxDq5/gMqXzpKgR6DAKBggqhkjOPQQDAgNIADBFAiEA0ew08Xg32g7BwheslVKwXo9XRRx4kygYha1+cn0tvaUCIEKCEwnosZlAckjcZt8aHN5zslE9K9Y7XxTErTstthKc";
// an RSA 2048 CA-signed cert
//const CERT_B64: &str = "MIICiDCCAi4CAQIwCgYIKoZIzj0EAwIwUzELMAkGA1UEBhMCSUUxEDAOBgNVBAgMB0lyZWxhbmQxDzANBgNVBAcMBkR1YmxpbjEhMB8GA1UECgwYSW50ZXJuZXQgV2lkZ2l0cyBQdHkgTHRkMB4XDTE1MTEyNjEzMzcwNVoXDTE3MTEyNTEzMzcwNVowgYExCzAJBgNVBAYTAklFMQ8wDQYDVQQIDAZJZWxhbmQxDzANBgNVBAcMBkR1YmxpbjERMA8GA1UECgwIQ2VydGl2b3gxDTALBgNVBAsMBExhYnMxDTALBgNVBAMMBE1pa2UxHzAdBgkqhkiG9w0BCQEWEG1zY290dEBpbmRpZ28uaWUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCjPBVwmPg8Gwx0+8xekmomptA0BDwS7NUfBetqDqNMNyji0bSe8LAfpciU7NW/HWfUE1lndCqSDDwnMJmwC5e3GAl/Bus+a+z8ruEhWGbn95xrHXFkOawbRlXuS7UcEQCvPr8KQHhNsg4cyV7Hn527CPUl27n+WN8/pANo01cTN/dQaK87naU0Mid09vktlMKSN0zyJOnc5CsaTLs+vCRKJ9sUL3d4IQIA2y7gvrTe+iY/QI26nqhGpNWYyFkAdy9PdHUEnDI6JsfF7jFh37yG7XEgDDA3asp/oi1T1+ZoASj2boL++opdqCzDndeWwzDWAWuvJ9wULd80ti6x737ZAgMBAAEwCgYIKoZIzj0EAwIDSAAwRQIgCDwgl98+9moBo+etaLt8MvB/z5Ti6i9neRTZkvoFl7YCIQDq//M3OB757fepErRzIQo3aFAFYjOooi6WdSqP3XqGIg==";

// USE_NIST384
//const CHOICE: usize=x509::USE_NIST384;
//use mcore::nist384::ecdh;
//use mcore::rsa3072::ff;
//use mcore::rsa3072::rsa;
// ** CA is RSA 3072-bit based  - for use with NIST384 build of library - assumes use of SHA384 in Certs
// RSA 3072 Self-Signed CA cert
//const CA_B64: &str="MIIElzCCAv+gAwIBAgIJAJA+8OyEeK4FMA0GCSqGSIb3DQEBDAUAMGIxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdJcmVsYW5kMQ8wDQYDVQQHDAZEdWJsaW4xITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDENMAsGA1UEAwwETWlrZTAeFw0xNTExMjYxNDQ0MDBaFw0yMDExMjUxNDQ0MDBaMGIxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdJcmVsYW5kMQ8wDQYDVQQHDAZEdWJsaW4xITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDENMAsGA1UEAwwETWlrZTCCAaIwDQYJKoZIhvcNAQEBBQADggGPADCCAYoCggGBANvNO8ahsanxzqwkp3A3bujwObJoP3xpOiAAxwGbW867wx4EqBjPRZP+Wcm9Du6e4Fx9U7tHrOLocIUUBcRrmxUJ7Z375hX0cV9yuoYPNv0o2klJhB8+i4YXddkOrSmDLV4r46Ytt1/gjImziat6ZJALdd/uIuhaXwjzy1fFqSEBpkzhrFwFP9MG+5CgbRQed+YxZ10l/rjk+h3LKq9UFsxRCMPYhBFgmEKAVTMnbTfNNxawTRCKtK7nxxruGvAEM+k0ge5rvybERQ0NxtizefBSsB3Q6QVZOsRJiyC0HQhE6ZBHn4h3A5nHUZwPeh71KShw3uMPPB3Kp1pb/1Euq8azyXSshEMPivvgcGJSlm2b/xqsyrT1tie82MqB0APYAtbx3i5q8p+rD143NiNO8fzCq/J+EV82rVyvqDxf7AaTdJqDbZmnFRbIcrLcQdigWZdSjc+WxrCeOtebRmRknuUmetsCUPVzGv71PLMUNQ2qEiq8KGWmnMBJYVMl96bPxwIDAQABo1AwTjAdBgNVHQ4EFgQUsSjrHeZ5TNI2tMcQd6wUnFpU8DcwHwYDVR0jBBgwFoAUsSjrHeZ5TNI2tMcQd6wUnFpU8DcwDAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQwFAAOCAYEADlnC1gYIHpVf4uSuBpYNHMO324hhGajHNraHYQAoYc0bW4OcKi0732ib5CHDrV3LCxjxF4lxZVo61gatg5LnfJYldXc0vP0GQRcaqC6lXlLb8ZJ0O3oPgZkAqpzc+AQxYW1wFxbzX8EJU0stSwAuxkgs9bwg8tTxIhDutrcjQl3osnAqGDyM+7VAG5QLRMzxiZumyD7s/xBUOa+L6OKXf4QRr/SH/rPU8H+ENaNkv4PApSVzCgTBPOFBIzqEuO4hcQI0laUopsp2kK1w6wYB5oY/rR/O6lNNfB2WEtfdIhdbQru4cUE3boKerM8Mjd21RuerAuK4X8cbDudHIFsaopGSNuzZwPo/bu0OsmZkORxvdjahHJ0G3/6jM6nEDoIy6mXUCGOUOMhGQKCa8TYlZdPKz29QIxk6HA1wCA38MxUo/29Z7oYw27Mx3x8Gcr+UA4vc+oBN3IEzRmhRZKAYQ10MhYPx3NmYGZBDqHvT06oG5hysTCtlVzx0Tm+o01JQ";
// an RSA 3072 CA-signed cert
//const CERT_B64: &str = "MIIEWzCCAsMCAQYwDQYJKoZIhvcNAQEMBQAwYjELMAkGA1UEBhMCSUUxEDAOBgNVBAgMB0lyZWxhbmQxDzANBgNVBAcMBkR1YmxpbjEhMB8GA1UECgwYSW50ZXJuZXQgV2lkZ2l0cyBQdHkgTHRkMQ0wCwYDVQQDDARNaWtlMB4XDTE1MTEyNjE0NDY0MloXDTE3MTEyNTE0NDY0MlowgYQxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdJcmVsYW5kMQ8wDQYDVQQHDAZEdWJsaW4xETAPBgNVBAoMCENlcnRpdm94MQ0wCwYDVQQLDARMYWJzMQ8wDQYDVQQDDAZtc2NvdHQxHzAdBgkqhkiG9w0BCQEWEG1zY290dEBpbmRpZ28uaWUwggGiMA0GCSqGSIb3DQEBAQUAA4IBjwAwggGKAoIBgQC6SrDiE4BpTEks1YpX209q8iH0dfvhGO8hi1rGYFYnz+eeiOvPdXiCdIPVPbGwxQGMEnZQV1X0KupYJw3LR2EsXhN4LZBxnQZmDvUXsTU+Ft/CKZUxVoXpNMxzwl70RC6XeUpPxvdPXa78AnfLL/DsOKsxCfNaKYZZ6G53L6Y69+HrCbyM7g2KrZ9/K/FXS1veMpRj9EbA6Mcdv1TUDNK2fTDV952AQO3kC3+PqywdVgPvntraAoQomrni+tcFW7UXe2Sk7DRcF/acBSuo2UtP3m9UWNL+8HOXvtRqmhns55Vj4DxKuPln759UBS7WZ11apCvC3BvCHR/k3WRf9PQWnW2cmT73/kEShvTRi8h7F9RWvYTEF1MuwSVy+l51q8O3rJU4XxnLm/YbtIGXZUf5Rqb0985zQkA+6rip/OSc8X5a3OV3kp38U7tXJ5sqBMg9RdIIz42cmiRLG5NYSj0/T6zjYEdwj3SYEBoPN/7UGSmhu8fdxS7JYPNpOsgeiu8CAwEAATANBgkqhkiG9w0BAQwFAAOCAYEAyxxEg0hWLFuN2fiukX6vqzSDx5Ac8w1JI4W/bamRd7iDZfHQYqyPDZi9s07I2PcGbByj2oqoyGiIEBLbsljdIEF4D229h2kisn1gA9O+0IM44EgjhBTUoNDgC+SbfJrXlU2GZ1XI3OWjbK7+1wiv0NaBShbbiPgSdjQBP8S+9W7lyyIrZEM1J7maBdepie1BS//DUDmpQzEi0UlB1J+HmQpyZsnT97J9uIPKsK4t2/+iOiknl6iS4GzAQKMLqj2yIBRf/O44ZZ6UZIKLtI4PCVS/8H5Lrg3AC0kr4ZkPAXzefUiTwyLVkqYSxSSTvtb3BpgOxIbmA6juFid0rvUyjN4fuDQkxl3PZyQwIHjpz33HyKrmo4BZ8Dg4JT8LCsQgd0AaD3r0QOS5FdLhkb+rD8EMSsCoOCEtPI6lqLJCrGOQWj7zbcUdPOEsczWMI9hSfK3u/P9+gOUBUFkb0gBIn3WvNuHifIHpsZ5bzbR+SGtu5Tgc7CCCPyNgz1Beb247";
// an ECC 384 CA-signed cert
//const CERT_B64: &str = "MIIDCDCCAXACAQcwDQYJKoZIhvcNAQEMBQAwYjELMAkGA1UEBhMCSUUxEDAOBgNVBAgMB0lyZWxhbmQxDzANBgNVBAcMBkR1YmxpbjEhMB8GA1UECgwYSW50ZXJuZXQgV2lkZ2l0cyBQdHkgTHRkMQ0wCwYDVQQDDARNaWtlMB4XDTE1MTEyNjE1MzU1M1oXDTE3MTEyNTE1MzU1M1owYDELMAkGA1UEBhMCSUUxEDAOBgNVBAgMB0lyZWxhbmQxDzANBgNVBAcMBkR1YmxpbjEQMA4GA1UECgwHQ2VydGl2bzENMAsGA1UECwwETGFiczENMAsGA1UEAwwEbWlrZTB2MBAGByqGSM49AgEGBSuBBAAiA2IABJ1J+FT5mxxYEM4aYKM0CvZHmh8JFXzoBmzibabrvyTz79+1QOrR+6MEEsKtmJIYPJi+GsQ0PmjF2HmJncM1zeQh7DQYJf2Xc8p5Vjd8//6YREBVfN3UIyrl87MSucy+mjANBgkqhkiG9w0BAQwFAAOCAYEAmuwa64+K1qlCELpcnCyWwMhSb+Zsw0Hh6q/BfxalZhsX1UFEwE9nHoVJcokaEEYF4u4AYXU5rdysRHxYBfgMbohguTT7sJwCfve2JqpqvhQOkGDG1DB4Ho4y7NPPYB2+UMd7JMD0TOcHXdgQ8FtAE0ClD8VkW0gAC0lCrbQbynfLoUjIWqg3w2g79hvdZPgRt208nFiHuezynOaEFePoXl8CxHInsxAnMaJn2fEs5/QH67pwD65mPdNFsvlr0zdzYcceqEmEHpRAXFOQAJtffGjWAGGX/CsghLuqlpdCiTGA1B53XoXKJvArr/kHpTNMsU1NnkQIHZ5n4USCo4QgL6n9nwem7U2mYBYjmxPi5Y3JJnTZz4zUnv0bD0vSwoivnFZox9H6qTAkeIX1ojJ2ujxWHNOMvOFb6nU2gqNZj2vYcO38OIrK9gwM9lm4FF20YBufh+WOzQthrHJv0YuQt3NuDQEMkvz+23YvzZlr+e2XqDlMhyR01Kk0MXeLGGcv";
// ** CA is ECC 384 based - - for use with NIST384 build of library - assumes use of SHA384 in Certs
// ECC 384 Self-Signed CA Cert
//const CA_B64: &str="MIICSTCCAc6gAwIBAgIJAIwHpOFSZLXnMAoGCCqGSM49BAMDMGIxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdJcmVsYW5kMQ8wDQYDVQQHDAZEdWJsaW4xITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDENMAsGA1UEAwwEbWlrZTAeFw0xNTExMjYxNTQ0NTlaFw0yMDExMjUxNTQ0NTlaMGIxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdJcmVsYW5kMQ8wDQYDVQQHDAZEdWJsaW4xITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDENMAsGA1UEAwwEbWlrZTB2MBAGByqGSM49AgEGBSuBBAAiA2IABOEPMYBqzIn1hJAMZ0aEVxQ08gBF2aSfWtEJtmKj64014w7VfWdeFQSIMP9SjmhatFbAvxep8xgcwbeAobGTqCgUp+0EdFZR1ktKSND/S+UDU1YSNCFRvlNTJ6YmXUkW36NQME4wHQYDVR0OBBYEFDQxIZoKNniNuW91UMJ1KWEjs045MB8GA1UdIwQYMBaAFDQxIZoKNniNuW91UMJ1KWEjs045MAwGA1UdEwQFMAMBAf8wCgYIKoZIzj0EAwMDaQAwZgIxANbml6sp5A92qQiCM/OtBf+TbXpSpIO83TuNP9V2lsphp0CEX3KwAuqBXB95m9/xWAIxAOXAT2LqieSbUh4fpxcdaeY01RoGtD2AQch1a6BuIugcQqTfqLcXy7D51R70R729sA==";
// an ECC 384 CA-signed cert
//const CERT_B64: &str = "MIICCjCCAZACAQgwCgYIKoZIzj0EAwMwYjELMAkGA1UEBhMCSUUxEDAOBgNVBAgMB0lyZWxhbmQxDzANBgNVBAcMBkR1YmxpbjEhMB8GA1UECgwYSW50ZXJuZXQgV2lkZ2l0cyBQdHkgTHRkMQ0wCwYDVQQDDARtaWtlMB4XDTE1MTEyNjE1NTIxMFoXDTE3MTEyNTE1NTIxMFowgYIxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdJcmVsYW5kMQ8wDQYDVQQHDAZEdWJsaW4xETAPBgNVBAoMCENlcnRpdm94MQ0wCwYDVQQLDARMYWJzMQ0wCwYDVQQDDARtaWtlMR8wHQYJKoZIhvcNAQkBFhBtc2NvdHRAaW5kaWdvLmllMHYwEAYHKoZIzj0CAQYFK4EEACIDYgAEf2Qm6jH2U+vhAApsMqH9gzGCH8rk+mUwFSD3Uud4NiyPBqrJRC1eetVvr3cYb6ucDTa15km6QKvZZrsRW+Z4ZpryoEE6esmD4XPLrtrOtoxtxFNRhiMmT/M9zcrfMJC5MAoGCCqGSM49BAMDA2gAMGUCMF0x6PAvvnJR3riZdUPC4OWFC2K3eiz3QuLCdFOZVIqX7mkLftdS8BtzusXWMMgFCQIxALJNMKLs39P9wYQHu1C+v9ieltQr20C+WVYxqUvgL/KTdxd9dzc3wseZRDT1CydqOA==";
// an RSA 3072 CA-signed cert
//const CERT_B64: &str = "MIIDFjCCAp4CAQkwCgYIKoZIzj0EAwMwYjELMAkGA1UEBhMCSUUxEDAOBgNVBAgMB0lyZWxhbmQxDzANBgNVBAcMBkR1YmxpbjEhMB8GA1UECgwYSW50ZXJuZXQgV2lkZ2l0cyBQdHkgTHRkMQ0wCwYDVQQDDARtaWtlMB4XDTE1MTEyNjE2MTYwNloXDTE3MTEyNTE2MTYwNlowYzELMAkGA1UEBhMCSUUxEDAOBgNVBAgMB0lyZWxhbmQxDzANBgNVBAcMBkR1YmxpbjERMA8GA1UECgwIQ2VydGl2b3gxDTALBgNVBAsMBGxhYnMxDzANBgNVBAMMBmtlYWxhbjCCAaIwDQYJKoZIhvcNAQEBBQADggGPADCCAYoCggGBAK5QhVjR+UGt3ZWPSGicpviqaOhxXmmvOepdl5Seqr+Iweb3IuEDgtHGwrw/EEgWlKPfS/2LW9ncptdNbVQh7+2rojj7ZtedrAK5p7I9b22f2U3sSHIqjtTT0BjqzL0qEwy/ATqbf93Tcr3yT0Ygh3yzbvn4zodrWQZK8kkN3PQKkiHBCuIxo+8MlTs8d99dl1hbJ84MYZuPmhrkB4oLEAt8+srtL+a4Yd0wPhuCYrLjBnYkD9TlcWLWWh8/iwXiznrY8gQsXSveQNzQjcmHilZrTlTL2dnyI2v7BAXXHSwo6UeES0n064fnYTr3JB0GArMcty6RD3E7xr64HNzzTE2+8cDxufNvU0tq2Z72oZ9cAReHUL5P6mLfORI+AhtCHrXGJch/F07ZX9h8UFpzok8NK5++Q7lHKuezTYRRPlDL5hDB3BUpBwvILdqujcbNil04cuLRBNT/WgqRXEBRjlHLgZaLChFV2VSJ9Z1Uke2lfm5X2O0XPQLhjMSiuvr4HwIDAQABMAoGCCqGSM49BAMDA2YAMGMCLxHSQAYP2EsuIpR4TzDDSIlsw4BBsD7W0ZfH91v9J0j5UWQJD/yNjMtyA2Qlkq/0AjB+SJQbLgycNJH5SnR/X5wx26/62ln9s0swUtlCYVtNzyEQ3YRHSZbmTbh16RUT7Ak=";

// USE_NIST521
//const CHOICE: usize=x509::USE_NIST521;
//use mcore::nist521::ecdh;
//use mcore::rsa4096::ff;
//use mcore::rsa4096::rsa;
// ** CA is ECC 521 based - - for use with NIST521 build of library - assumes use of SHA512 in Certs
// ECC 521 Self-Signed CA Cert
//const CA_B64: &str="MIIC+TCCAlqgAwIBAgIJAKlppiHsRpY8MAoGCCqGSM49BAMEMIGUMQswCQYDVQQGEwJJRTEQMA4GA1UECAwHSXJlbGFuZDEPMA0GA1UEBwwGRHVibGluMSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQxDTALBgNVBAsMBExhYnMxDzANBgNVBAMMBm1zY290dDEfMB0GCSqGSIb3DQEJARYQbXNjb3R0QGluZGlnby5pZTAeFw0xNTEyMDExMzE5MjZaFw0yMDExMzAxMzE5MjZaMIGUMQswCQYDVQQGEwJJRTEQMA4GA1UECAwHSXJlbGFuZDEPMA0GA1UEBwwGRHVibGluMSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQxDTALBgNVBAsMBExhYnMxDzANBgNVBAMMBm1zY290dDEfMB0GCSqGSIb3DQEJARYQbXNjb3R0QGluZGlnby5pZTCBmzAQBgcqhkjOPQIBBgUrgQQAIwOBhgAEAKUj6Qa4Vr1vyango8XHlLIIEzY9IVppdpGUrMlNfo0Spu+AXGhnluwJTZXOYLi8jSIPEAL7vuwS5H6uPPIz1QWXALRETVYAQfK0pIfPHq+edTHVTXMcAUpdNla2d4LwYO7HpkSQFHd7aaDN3yVhSL2J0LBLgy0wGkEHuyK1O2r0xNu6o1AwTjAdBgNVHQ4EFgQU966PLshKffU/NRCivMmNq8RiRkAwHwYDVR0jBBgwFoAU966PLshKffU/NRCivMmNq8RiRkAwDAYDVR0TBAUwAwEB/zAKBggqhkjOPQQDBAOBjAAwgYgCQgHkLczeTWXq5BfY0bsTOSNU8bYy39OhiQ8wr5rlXY0zOg0fDyokueL4dhkXp8FjbIyUfQBY5OMxjtcn2p+cXU+6MwJCAci61REgxZvjpf1X8pGeSsOKa7GhfsfVnbQm+LQmjVmhMHbVRkQ4h93CENN4MH/86XNozO9USh+ydTislAcXvCb0";
// an ECC 521 CA-signed cert
//const CERT_B64: &str = "MIICZjCCAccCAQMwCgYIKoZIzj0EAwQwgZQxCzAJBgNVBAYTAklFMRAwDgYDVQQIDAdJcmVsYW5kMQ8wDQYDVQQHDAZEdWJsaW4xITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDENMAsGA1UECwwETGFiczEPMA0GA1UEAwwGbXNjb3R0MR8wHQYJKoZIhvcNAQkBFhBtc2NvdHRAaW5kaWdvLmllMB4XDTE1MTIwMTEzMjkxN1oXDTE3MTEzMDEzMjkxN1owYTELMAkGA1UEBhMCSUUxEDAOBgNVBAgMB0lyZWxhbmQxDzANBgNVBAcMBkR1YmxpbjERMA8GA1UECgwIQ2VydGlWb3gxDTALBgNVBAsMBExhYnMxDTALBgNVBAMMBE1pa2UwgZswEAYHKoZIzj0CAQYFK4EEACMDgYYABAAva/N4kP2LMSGJZ5tvULlfdNx2M/+xYeCrQkuFmY8sG+mdcUAaSx819fztn2jz1nfdTJnuj79AhfUOL8hlTW14BwErp3DnqWa7Y/rpSJP+AsnJ2bZg4yGUDfVy/Q0AQychSzJm2oGRfdliyBIc+2SoQJ/Rf0ZVKVJ5FfRbWUUiKqYUqjAKBggqhkjOPQQDBAOBjAAwgYgCQgFE1Y7d9aBdxpZqROtkdVNG8XBCTSlMX0fISWkSM8ZEiQfYf7YgXzLjk8wHnv04Mv6kmAuV0V1AHs2M0/753CYEfAJCAPZo801McsGe+3jYALrFFw9Wj7KQC/sFEJ7/I+PYyJtrlfTTqmV0IFKdJzjEsk7ic+Gd4Nbs6kIe1GyYbrcyC4wT";

const SHA256: usize=32;
const SHA384: usize=48;
const SHA512: usize=64;

// convert from base64 to byte array
fn frombase64(w: &mut [u8],b: &str) -> usize
{
    let len=b.len();
    let mut k=0;
    let mut j=0;
    let mut ch: [u8; 4] = [0; 4];
    let mut ptr: [u8; 3]= [0; 3];
    while j < len && k < w.len() {
        let mut pads = 0;
        let mut i=0;
        while i<4 { 
            let mut c:u8 = 80 + b.as_bytes()[j]; j+=1;
            if c <= 112 {
                continue; /* ignore white space */
            }
            if c > 144 && c < 171 {
                c -= 145;
            }
            if c > 176 && c < 203 {
                c -= 151;
            }
            if c > 127 && c < 138 {
                c -= 76;
            }
            if c == 123 {
                c = 62;
            }
            if c == 127 {
                c = 63;
            }
            if c == 141 {
                pads+=1;    /* ignore pads '=' */
                i += 1;
                continue;
            }
            ch[i] = c;
            i+=1;
        }
        ptr[0] = (ch[0] << 2) | (ch[1] >> 4);
        ptr[1] = (ch[1] << 4) | (ch[2] >> 2);
        ptr[2] = (ch[2] << 6) | ch[3];

        for i in 0..(3-pads) {
            w[k]=ptr[i];
            k+=1;
            if k==w.len() {
                break;
            }
        }
    }
    return k;
}

// pretty print
fn printoct(preamble: &str,o: &[u8]) {
    print!("{}",preamble);
    for i in 0..o.len() {
        print!("{:02x}", o[i]);
    }
    println!("");
}

fn printout(preamble: &str,o: &[u8]) {
    print!("{}",preamble);
    for i in 0..o.len() {
        print!("{}", o[i] as char);
    }
    println!("");
}

fn printdate(preamble: &str,o: &[u8]) {
    print!("{} [20{}{}-{}{}-{}{} {}{}:{}{}:{}{}]\n",preamble,o[0] as char,o[1] as char,o[2] as char,o[3] as char,o[4] as char,o[5] as char,o[6] as char,o[7] as char,o[8] as char,o[9] as char,o[10] as char,o[11] as char);
}    

fn main() {   
    let mut io: [u8; 5000] = [0; 5000];  // reserve some space on the stack
    let mut sig: [u8; 1000] = [0; 1000];
    let mut cert: [u8; 5000] = [0; 5000];
    let mut cakey:[u8; 1000] = [0; 1000];
    let mut certkey:[u8; 1000]= [0; 1000];
    let mut hp:[u8; rsa::RFS]= [0; rsa::RFS];
    let mut hh:[u8; rsa::RFS]= [0; rsa::RFS];

    println!("First check signature on self-signed cert and extract CA public key");
    let mut len=frombase64(& mut io,&CA_B64);
    printoct("CA Self-Signed Cert= \n",&io[0..len]);

    let mut st=x509::extract_cert_sig(&io[0..len],&mut sig);
    if st.kind==0 {
        println!("Unable to extract cert signature");
        return;
    }
    let mut slen=st.len;

    let mut r=&sig[0..slen/2];    // borrow memory as a slice..
    let mut s=&sig[slen/2..slen];   
    let mut rs=&sig[0..slen];

    if st.kind==x509::ECC {
        printoct("ECC SIG= \n",&r);
        printoct("",&s);
    }
    if st.kind==x509::RSA {
        printoct("RSA SIG= \n",&rs);
    }

    if st.hash==x509::H256 {
        println!("Hashed with SHA256");
    }
    if st.hash==x509::H384 {
        println!("Hashed with SHA384");
    }
    if st.hash==x509::H512 {
        println!("Hashed with SHA512");
    }

    let mut c=x509::extract_cert(&io[0..len],&mut cert);

/*
    let mut cptr=0;
    let myctlen=x509::find_cert(&io[0..len],&mut cptr);   // find cert slice 
    //let pklen=x509::find_public_key(&io[cptr..cptr+myctlen],&mut pkptr);    // finds raw public key slice, starts at offset pkptr, of length pklen
    let ccc=x509::extract_public_key(&io[cptr..cptr+myctlen],&mut cakey); // find and extract public key
    printoct("EXTRACTED XXX PUBLIC KEY= \n",&cakey[0..ccc.len]);
*/
    printoct("\nCert= \n",&cert[0..c]);

    let mut certificate=&cert[0..c];  
// show some details
    println!("Issuer Details");
    let mut cs=x509::find_issuer(&certificate);
    let mut ep=x509::find_entity_property(&certificate,&x509::ON,cs.index);
    printout("owner= ",&certificate[ep.index..ep.index+ep.length]);
    ep=x509::find_entity_property(&certificate,&x509::CN,cs.index);
    printout("country= ",&certificate[ep.index..ep.index+ep.length]);
    ep=x509::find_entity_property(&certificate,&x509::EN,cs.index);
    printout("email= ",&certificate[ep.index..ep.index+ep.length]);
    println!("");

    let ca=x509::extract_public_key(&certificate,&mut cakey);
    if ca.kind==0 {
        println!("Not supported by library");
        return;
    }
    if !x509::self_signed(&certificate) {
        println!("Not self-signed");
    }
    if ca.kind==x509::ECC {
        printoct("EXTRACTED ECC PUBLIC KEY= \n",&cakey[0..ca.len]);
    }
    if ca.kind==x509::RSA {
        printoct("EXTRACTED RSA PUBLIC KEY= \n",&cakey[0..ca.len]);
    }
    let pk=&cakey[0..ca.len];
    println!("\nChecking Self-Signed Signature");
    if ca.kind==x509::ECC {
        if ca.curve != CHOICE {
            println!("Curve is not supported");
            return;
        }
        let res=ecdh::public_key_validate(&cakey);
        if res!=0 {
            println!("ECP Public Key is invalid!");
            return;
        } else {
            println!("ECP Public Key is Valid");
        }
        let mut sha=0;
        if st.hash==x509::H256 {
            sha=SHA256;
        }
        if st.hash==x509::H384 {
            sha=SHA384;
        }
        if st.hash==x509::H512 {
            sha=SHA512;
        }
        if sha==0 {
            println!("Hash Function not supported");
            return;
        }
        if ecdh::ecpvp_dsa(sha,&pk,&certificate,&r,&s)!=0 {
            println!("***ECDSA Verification Failed");
            return;
        } else {
            println!("ECDSA Signature/Verification succeeded");
        }
    }
    if ca.kind==x509::RSA {
        if ca.curve != 2048 {
            println!("RSA bit size is not supported");
            return;
        }
        let mut pbc = rsa::new_public_key();
        rsa::set_public_key(&mut pbc,65537,&pk);

        let mut sha=0;
        if st.hash==x509::H256 {
            sha=SHA256;
        }
        if st.hash==x509::H384 {
            sha=SHA384;
        }
        if st.hash==x509::H512 {
            sha=SHA512;
        }
        if sha==0 {
            println!("Hash Function not supported");
            return;
        }
        hmac::pkcs15(sha, &certificate, &mut hp, rsa::RFS);
        rsa::encrypt(&pbc, &rs, &mut hh);
        if hp == hh {
            println!("RSA Signature/Verification succeeded");
        } else {
            println!("***RSA Verification Failed");
            return;
        }
    }
    println!("\nNext check CA signature on cert, and extract public key");
    len=frombase64(&mut io,&CERT_B64);
    printoct("Example Cert= \n",&io[0..len]);

    st=x509::extract_cert_sig(&io[0..len],&mut sig);
    if st.kind==0 {
        println!("Unable to extract cert signature");
        return;
    }
    slen=st.len;

    r=&sig[0..slen/2];
    s=&sig[slen/2..slen];   
    rs=&sig[0..slen];

    if st.kind==x509::ECC {
        printoct("ECC SIG= \n",&r);
        printoct("",&s);
    }
    if st.kind==x509::RSA {
        printoct("RSA SIG= \n",&rs);
    }

    c=x509::extract_cert(&io[0..len],&mut cert);
    printoct("\nCert= \n",&cert[0..c]);
    certificate=&cert[0..c];

// show some details
    println!("Subject Details");
    cs=x509::find_subject(&certificate);
    ep=x509::find_entity_property(&certificate,&x509::ON,cs.index);
    printout("owner= ",&certificate[ep.index..ep.index+ep.length]);
    ep=x509::find_entity_property(&certificate,&x509::CN,cs.index);
    printout("country= ",&certificate[ep.index..ep.index+ep.length]);
    ep=x509::find_entity_property(&certificate,&x509::EN,cs.index);
    printout("email= ",&certificate[ep.index..ep.index+ep.length]);
    println!("");
    
    let ic=x509::find_validity(&certificate);
    c=x509::find_start_date(&certificate,ic);
    printdate("start date= ",&certificate[c..c+12]);
    c=x509::find_expiry_date(&certificate,ic);
    printdate("expiry date= ",&certificate[c..c+12]);
    println!("");  
    
    let pt=x509::extract_public_key(&certificate,&mut certkey);
    if pt.kind==0 {
        println!("Not supported by library");
        return;        
    }

    if pt.kind==x509::ECC {
        printoct("EXTRACTED ECC PUBLIC KEY= \n",&certkey[0..pt.len]);
    }
    if pt.kind==x509::RSA {
        printoct("EXTRACTED RSA PUBLIC KEY= \n",&certkey[0..pt.len]);
    }   
    println!("");  

// Check CA signature

    if ca.kind == x509::ECC {
        println!("Checking CA's ECC Signature on Cert");
        let res=ecdh::public_key_validate(&cakey);
        if res!=0 {
            println!("ECP Public Key is invalid!");
            return;
        } else {
            println!("ECP Public Key is Valid");
        }

        let mut sha=0;
        if st.hash==x509::H256 {
            sha=SHA256;
        }
        if st.hash==x509::H384 {
            sha=SHA384;
        }
        if st.hash==x509::H512 {
            sha=SHA512;
        }
        if sha==0 {
            println!("Hash Function not supported");
            return;
        }
        if ecdh::ecpvp_dsa(sha,&pk,&certificate,&r,&s)!=0 {
            println!("***ECDSA Verification Failed");
            return;
        } else {
            println!("ECDSA Signature/Verification succeeded");
        }
    }
    if ca.kind == x509::RSA {
        println!("Checking CA's RSA Signature on Cert");
        let mut pbc = rsa::new_public_key();
        rsa::set_public_key(&mut pbc,65537,&pk);

        let mut sha=0;
        if st.hash==x509::H256 {
            sha=SHA256;
        }
        if st.hash==x509::H384 {
            sha=SHA384;
        }
        if st.hash==x509::H512 {
            sha=SHA512;
        }
        if sha==0 {
            println!("Hash Function not supported");
            return;
        }
        hmac::pkcs15(sha, &certificate, &mut hp, rsa::RFS);
        rsa::encrypt(&pbc, &rs, &mut hh);
        if hp == hh {
            println!("RSA Signature/Verification succeeded");
        } else {
            println!("***RSA Verification Failed");
            return;
        }
    }
} 

