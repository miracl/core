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


extern crate mcore;

use mcore::x509;
use mcore::dilithium;

const CERT_B64: &str = "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";
const SECRET: &str = "MIIXWgIBADANBgsrBgEEAQKCCwcGBQSCF0QEghdAKErqaBp0q+OJm40BVmafKbmMRGuOe7r68GHDg/FCE4gM3Q342+IjzecJyw7LFEUp9ClQCbgcJrG0fR8bCeFgqXhjIqoGaU0efj48iABLq9AlvQ2vHR1qmJJze9siOIwLMHgoEwAWFQBSGEgSZ4FjcTYYI0ZiJFiAVgRGRGNVBRBRYYRzExdxIQV0V0ImIwCChBWGF4ISchRQQYg2CIFIRDJTNYdXI1IjYWMQiAYCOAEhEHhgUmYQEVYAFlhhIWaGeEBHhhiIY2VHZxIQZgUDE1OASCRIBUh3hSB1EjBWQlh0YhQSA4RIJIiCAwRoNAZhYAgQZ1MIYHh2QAFBdYZABRZ0BnIEEnZCB0A0UlQBcHUicnUiF3UkRFNReCSFYWJ2AxMhNBUhYiRHBYZYVXgHNwhIhRFoKBFkJHJ0BnFWOIVFRGBCEUQYUUNFchFQRodYiGQDBRJ2hDRiUAgGcYMEZBeDdxdwdiIoSCBFIYcGMxgoVkI3V3ECYwgUJnF4dnMDV0YwEHNYUlc4QoOAh2YBcwCAaDNTcFhCNoGHMSJBAScSBnMzJ1hSIoU4VlRwJxUjdHdRBFiHR0RmQiIycgYjhHViMHeAggGGYXcSBFMYJyUTFiBzgxVWiAcUAHaBFBUyBBKHAFIDEiVwcQY0MkEjNkNzVwEyhgNIcGIyCGWAFDNxcyhxgCYxeFhXcCRiVUOHV1JwZTQXYSdTNEYSQFdYBYIYYxVAcgZYMjBwEzNChGAWgUFnRVdVRoYANTFoEmczaGYHaAJDhQVzAnZhdVdgBINVUhUFEichYSBEV1R1ZlgGAyNhVmFUFyd4MxJAA0ZSeDNlMocEgyAEJDMQR3FRFkUyRoKFhmGEU0goCDNzNSdoQzgmWDAyBVAVaHYwOHBigwU1YAUmhVVHUxiFFIAhFDhYRRZhJgZYUDFSiHhWEHdGNogUEoFzgEFDVzIFUYQTU1RyUWMjBAUiQTEUdUUDBwMgUQSIVIdxOEQYEARIVxcjViVTQmIQB1MzASA3UoWIMwRwiDg1BEJyVzR1SBgxM4BRQBMhBDEhAAgkUXJRVUFXIBBQYmdoZ3YhQlYBhRMUgTVkhmQkKBCDd2CBMjMUQoYhaDeFc4B4cFCEAiNhB4QRJBJXMTYBIyRUVUAVWIcoJUM1ZhUnFlWHJzYjVVgWYBcxOFCDJFcQhBg3EkeCEBMxVyE0KEGEYxSHSBcEYXgAAIZlaAEgQBdhVheGVGRgEAZmJFNQSHFWVSJAcXhFh0N4YhFAZRY2EEhgVWUCBBFFEGMDA2U2B3gCInMmSIVFdoRgQFExAhR4MyE3gRZ2EzdxIoR1NThGARRxEWMoAYdxE2BYdxhGVSAxOGJmgyJWVlModnICQ1YVdHKDdydTcRdFNRIhBRYVNnUyEyhFURhHAEMViAiBFUSAdSSFFjBlZRJ0JII3MTYjM1ZANIaAI1clMFJ1MXdBcjYEYgMRQhcjYoJzVXGIYndkCDERM1YRVlQQiHJiZhYVGDWAEhFBRyWGJRcEMWYxEXUCg0UjiFMUADUmYndnY1IEBzATJ0IRdHRGcnV4RCJwEWJUESQYNoOEIhhoUBMncYWBAAMhiASINyZlEYZ0RyUneGMGUTMwaHRhAigkIIeHFHgIQBJCVQglhQBjRhhIhVMCYBExhjNRUySEREAjgIhkQjUIaCeGVjADCEaCIlZiMgAYEmBhMYBXh0FTVVRFggQThSRnQ1h2B1hmUWYoZAckIzM4E1Yng1QgZUhGCAYwYUQyRCYzB0BRWAQRVQYgRECBFFY2BRgRUGdwMmUWJDcEBFRININYQmhghFVINSJTESVHMyOHBzZFglBlZFMHUTBGVHRlMWdIJVYzMTIGRmAwZ3V3BERhR0gSAwJkMXMHhSGBCHVgQiZlKBFmM0JkGFQwOFVlZ2ZQGChAVCBQYFhXAjKFRmFjYCaCgTQROHhRhHA4SBQnF1JHczZIEEJ4NXgzgVM0iChFZwJzEyJAiAVBJXhUYAV4QDAivNAx9CyLw0PZFXCbRoHNJxhiQ9k+UKJLqyHaJQcUBDjRKUdzTLkQJSPCkWy1gIph6Itm3R/3pu4tlfetAfB2HkOUmRwPaD/VNPcmaFbiIPxBSIvG0O9UN+KtJakjiO7+3GLMkJB530lSqPwKbSZloKvGgkb+gANZtEr32u49rZKIl/B6enjMc9WniFHfoaOtrfN5Mu+4jxEONFhPNbWAkvMFnuOsKN9T7TrtptO/w+Pfg2o9tJtCOH9zE/IPvdYPhteFFc1+YGAQiGYkgtGP9P6ayNhBEZDQMx5OJhRCjVAjBAGeOEpFXYAgm0axxKoQWnjEd17U1MlKLAiR+M1EkhF2IXw7lUZ3lXWbdLJDNrLzAKHj3/3zNgmbgVVWkF4okpwHEiVEAhoBG2ynD4lSv8STa01ndfyJDrnbLKZ6j1XxW+Ng9Cz4p6hWb70d9lELMzUKb5R5tIcT4kV9b6x1gZ4BFhDzhABTj+nBkJWU+oOqQPTQXTTtoNvhERzqmpu5Y7DaPPpMbSoZa9pfoaPvuTBQIChQGPdb/BLBRX5bL+XopE2TsrzkpKqS3jjaAojopQTzpQLd7N5F15ckc2leQuMeIrxVdB0P/NrFjg3ljeHGxKpN7QvpK+BOYbVb2/qE6tOB7tWS1Y6FSE6VsJfqEKJhFWKIMYSV2tY3L/jl222yjRrkfnzXCVmNeWiucUEHXq60vlxZ5dAnH/Swrm5glj3z76brh8Erf0atH2wauTfbpvPbJ2AyFVN4mHxgTuA4nptQqg7HvNhc07qOHethmvTxyC3Q6ViSbnGJT1oUKQR8CrRNd7Shji2Yt7i2CGVeP12v0ZLTPQD6vHY7MUUBP6se0AiQdUrOOaI/vwlqboEmtOvFoHxHEBr07iEvTx6fC2Dhy1ujSPeM6ZiM0yDDXPL9o8VFDtTPGP/7O2vkpfMDa4bfJacEpBkN21YL8UZeIe8qWhIJ5jPkbUuu+TpHWhzUCAflE4T1ls29o65pPlDZU8WNvu3F25SJgprQyxNnkOIQmGGTPAQIxp5PUu10XDeS6h+gm6Qvk1EzqX/6ZuBuZcXqMhAMkGp9B/oPo3PJk08T6+5UraQ3fFrlRn2zVIjVhbE7j4hm469hogqq3/DGzokBRvgxV7QKXyYOIFruAXezYVHXGuJJ4zu7W28UZiKFGNr86Ne97BAgkgOzmt7oS/SGXmjFUEgnnuG9l9qmuLHfpxK+6wIpehIKBVBLOx93hLxRgAOVSb3fLn1TaBPbfw5R2nLG7cmvoRmIrbyRow+CvBJgAoQEBfoTXcdPDU75/iMegBFk85O+AWP/KcCM6o/zEy/Kmxj+WIONFxLpLlJ9iJP76lZ9URLoe9wLybODU8uGFfnRECWoLFSUfAUU9fdcaCzoLIYj+Mu5vQZk+veapB16RA/VaRNokcBmgDAezZIa/2gebkFg20OGleT4du+yih6t8oSmcy0e/U2dRyOAdb+oHglhzHzZy2REv6V1Lnep2ZoBKeBhtVnmzkXhsr1C7sU/6sB2Vrf+XXJgJX7g3YGC5wAsAEzRjB1jjZyncZxRVwudDHVfKiwev0LJ6jUMEM+Qg6d0h1+qcC95Je2uhC2PNV/atuP+2hjXkGE4ydpzS5Z4YySGBAiIj9Gybkok4ITjuc8Rlwvxl76bkFSSsPPam9FV3/UpP7yegvOCs56NuOoSJUvJ5396SxxeDobNTlyxZuCrFXZk6M7iSkMabSFouhX+jgwqUsn3FitJLR88w2e7yLIU8UjhtHjoYhYlpBcQKGr4BeTkI3M9OCaZ1dwrpqj913bQKxz9HlwEfvop3rpGm6tFUDmqmuzO9CYMZv0xwtUzS1iCRRaA4ABUEPf9ZAFI5Ztxn0+t55OcCUT/O9FkLweRsQ69vgtGOIDYoCxTYcMoYaXxUaRNTiCcTYiYvjmpq4x9KsKmBfZq/O8ictmOulQm4BTktio2ACjn9P0V2ehHy3PO0IYHDyYitNIdwp9ZAN/sAVGAAczhXOCYJvmD3lYPVzFEH9l2ziNwmcOh+ulCT7wxeOQzIWUNIT85Qh9PHGQVWoywp3IA2OYV+G99A2BaKKDWmShxMlA7rTW73nthiCI1DXPe8dHyTKFcBdrt0L2WtbdJaqFlZsye1UK8i6mVJTnMsIgJMoTb8Ys4PINAUSQZdYdck1uj+MwzdbOhmkIY2Ih+z0kmAnsfhsP56wXiz2Z0svlyEBE2j9cgz77tdc4CUNgFC08i9JAkHy7wl8psdpqXF7bN+mxHpzhNyL4+Ti24Z5idjEiNMw+1XOOGqDcMdMEcDNmzdPmSczYeOlei1H0RKqtfxuGSrQEzjA8lf31IhRcisBmPn2Obc8equCrZxfLuBj/gM9dh6up6xuXiF3GtvBNBMXXMIccsvZjUgrwcd3BsCHCNLc/gGMI1N120ecTlPwJyrtdLET9WnwGOau8jS8Nb4EL/nASo6d7tSWMED4qa+5K0gFHJ0ytJzQc1pPfICqdZ6MFR/i2SeY/4ksJhiojAV/m4lgBlfVcu1UwFK693rlbPFL4fcYg2cIAWZ8/JR62MYGIaLnqLlZTLOOJ4cikx7Ns9GlBVYPQ2tskyABM6ulNKW+tIxa+q1SM3tsAbiUqOQtx4mahhdktOk8KsUTnJIsN9doYDh9pEGCS4Sw3M6pLVSh37g/g7MeGEpAgjfXAnkaMIymjjuL0XcppgvK0ZEWENRq/vR5vJUAp3Gc9NCpYpsWF+sRUtIqhlAlEUyKtY1PRdr6XtPW8P/7Pr8VGIHnjW9ZyORGdzk8YsKKpp0rZrKVPSxaAtJDGL7gRXaYUywMU5to+qUhbMTRAbrsW/sfUk9YgtO9cdVjuXcmccVY89pflmy+0zxkqvdRcfKNlcl9k0S8NSY9yvaYToPXJ6ImU7IA3Cingib5JqsJPn9xw5cZ3MGQZ8S81+qhMCmweG4it77k81HZOOinmDnfH8g0O3XQtPGlQuJv9yE7WUJRQ++K60mpstfvWWcLe2FYr55+fUbjdX0CuRwbtxXzZ2Gn/tG1dnPIh3fdbaTqjZoa7AihKROhwB3ku0kHsL0/fH4XdC+7vGDtx2M20TE/tr7NL8ioU/QRm1Noe8D3YhaqFxGXRqECmdf85hYeYpdlGX6nPBujybAT84+0Zrsh3OCpDZTFyaZhpFBzAWIUgvanui45DDosWtVGlm+mJJ16/HuxH3gF3np1un2aspJgM8MAn0XYlSAuNEjGIhdiS5NrIvQi1+GnTN607yMedFCwkcnDCvifrJRX1/yixdqpVaIAgU1uZojewLUShiXYdomm4BxDCqg+zUk3m6xyhK6mgadKvjiZuNAVZmnym5jERrjnu6+vBhw4PxQhOIanfkYAiPBP9RatatMUVSfqfVptUmg50Ms4A8ebV/V3icYoRr6+TJEu0y+fcyZNTWCVlhNjqIGcz5/ABrNfY71NhmGdSdbsEkXer+TV1lnu6Oh2PilBZqHmJjua5RfpLnhij0E9blmT1eUl+MSJe/heMr000BxdrvUs/1Fu5+NaOIEZuLf9HWrZiu2yQWC0M+FRiD1XS/mml6GxgCsbMNt3+JLrxWPCT94XUTiRp3Zgj6mk8GaeiTbfczod3xFSzDi7R2UZk88zLARfdnf9sktC6IrnFSh7E7dS4YX2uvK+naaaNFLkDvv+9b8dsm7iXPPxHUXko7/sdAILdpeeFe9IhHPPpAN9KlTrbP56ZH3AvYx9bmzF2l/l2bYaVZ/gVeYBiNcCMppNAQP40naez9j/CfOb7lBmJ1AM+3UngsidHjjfuMdnfCldJGzyZaJMRMCIOjm9DoYLDYDeboKrhxhcQdQ6+X1seiqSAHUy3H9ozlvP7+dPGsYCNv2cJeZ2J203FytDx6MBrMvWKWOu8m8wGs8KdM6FFsIfwQWGpfGmX8dK9nYLTiP41doXVHpeX4pOA4jqc7NkyXLD6UN0c5+RrV6QaWcO0IlUYj4n0VtIhLzxH2WovmVR1PVz8aVgjWDvPywXPT3jtugd15eZlnJu4OfnS1OFawf8Vsjym+juLwoUykpARZOeHEvIb+xs5sWpUlSQzhz9eUkk++cL2NmI0gdFPmKGb31FV3hurshz4EWKpqjTGL5ZwmSGu4Z8L5d7JL6Z7T1OCoym2R1+wiaWl3JBeITk9ho4R4eOoRnGy7UOCmUUG6XDvuEJrRHbbfWJqfSdzM+QybH5XKkpOWgNq8P632xBWB6vocO4QnAnm9vlV/w3i78oRnWFdQlfgKL/9DLFJ1httRlmeaZ9Sm1xM8e9RaqVCMjCWl7MhANyH0hWPZ3pzqkVnKlElS3V6B+98YCAJc8O9OOySeUGU4j7/yD1hc9g1iDCXrdTk4ubd8KEawOhOv64aNeSPl/rh4ZI2z2z9fGW6UpD/UqtCYufn782LWULXwaVtPchH4uhGL22HGU5SR+y5Bw7UH/oWTs9wKR4CIXUI2oyyedtUkueGqOQJ8x4vRImbJbSsEqOoquGX/Zs+t+Bh4y70N2zJqzYUxInx4CeHDPhRi5tj6cwcmNaB7Z1ccVoYIEs9D0P3fSwZeJ4ohiPEZ0yJ6Y60hiIJ1uSzQy+TuNrfh72jBIJEwvPDevxk39B6VmntkU3PlN0PKdjL4eNYcBTfkts79GpRQoK2AFg0C+9NKvNck1TqaRcIdh62LYuRtklVjCBXB63AckZIc5o7t81KYJaHboHOpkw4Isd22iCmZ81Jo9nPucXmTQ7CKZz6qiTAS1EoYzxDS6rtMfrd25gB0Qx83urx0jDtsjH4taoGwQV0Wpvxl1f1OSebilq+jjVbKW2J0t8TP5XrU3U69DT5FwfDNOjkTky2TzaWfWcg6kdGZguWv4mZV1XmL6jmcBXAGM4quY5AUILfrwBpWpxkVofEUiQ6bSG66FrXVZFA19Zw+UKATvGLBU0X96tdArae9oTSWCdkpbSgswretT/PZb9CO+WT2+Hqd0xCoknE+857aamiLQ+tHaaA6698cw9Otv6wc7UGCmfxoWx2Xh31nER+g/O939jqn1J0eYUHN/ZJcLEjH1291j+jSBxSD4tXnRrUDb8eiF1J7IoGAfhfElcK1Knon26td2I4J/5S07wySs2UnQIeGfGkUgjtY3xvf33ioOD+6l4aJnChQlwRo2o3x3HUNSxfygrqYaAKAxWetxq/EhDTYfGLMjtNoAjiidmF2EXQ7jRjaVM+eMNPYzZDVL9Nh/Jz6m9HkOAElyeYJson++aR6CGPuWVhFZJtBawDv++vC2QTCCD0HKIPjAvYkJ6fidhYPn4NKVirJkM0WNMZS5hr1XxD4MnLBxoL6FuEfP8mg06jbkrwlnZH2VgZGOxcG3dOPrHf/ee+EjLm1yROyQrgDSB1EghUpuT9Rbh7bSzwYUim3c9lBx160zbGQtv7is1L55/aFqpLd6IWBLpiVWYVdpUVK6pHqZMe8KlNFS+4lJcarKf+IQQSPrzAqP/lXsmgqS2Sr1OUJAu35AKvVH/IPVxz4CPaSAvXJeLYGPYahXZOL9g43KVSNhjtLXNAcjGGjjON864ptKNTsrhaE3UXo8tkG7BqdXW+Hlb9V8ndxg84t9V7DOxvf1Nqw4LOqvSwQvZv4MxVpQ1vkt1IlJoQrx/4tS+Eme+1L9HY4KaX12ExpQXICB5xPsPRXxDbbQutGs+8eIpP0LV70UjoHvK5/aeKrmzIReC87KuXLTQkjSBmnHnjAA4nUgvFSBW9WKEGwVx73c0hJMGnyRLeyYCR2Nts1FY+qTPbs9BN0ivGT0PKACrgW5cL0sgAY72oQwFlUAuOTDjZOwVdI5C3/qusGONfH06lKOy+7hDJZiwfmfdvQfAShGIewzH9gdbWtNNn0z6/6xSezlWW/cfAF+NntZL8ruWoaUHEl8aCG6eB8/VWg7rYNd5J9ucZe";

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
    let mut io: [u8; 10000] = [0; 10000];  // reserve some space on the stack
    let mut sig: [u8; 5000] = [0; 5000];
    let mut cert: [u8; 5000] = [0; 5000];
    let mut cakey:[u8; 2000] = [0; 2000];
    let mut certkey:[u8; 2000]= [0; 2000];
    let mut keys:[u8;6000]=[0; 6000];

    println!("First check signature on self-signed cert and extract CA public key");
    let mut len=frombase64(& mut io,&CERT_B64);
    println!("Signed Cert length= {}",len);
    printoct("Cert= \n",&io[0..len]);

    let mut st=x509::extract_cert_sig(&io[0..len],&mut sig);
    if st.kind==0 {
        println!("Unable to extract cert signature");
        return;
    }
    let mut slen=st.len;
    let mut rs=&sig[0..slen];

    if st.kind==x509::PQ {
        printoct("Signature= \n",&rs);
    }
    println!("Signature of length {}",slen);

    let mut c=x509::extract_cert(&io[0..len],&mut cert);

    println!("Cert length= {}",c);
    printoct("\nCert= \n",&cert[0..c]);

    let mut certificate=&cert[0..c];  
// show some details
    println!("Issuer Details");
    c=x509::find_issuer(&certificate);
    let mut ep=x509::find_entity_property(&certificate,&x509::MN,c);
    printout("My name= ",&certificate[ep.index..ep.index+ep.length]);
    println!("");

    let ca=x509::extract_public_key(&certificate,&mut cakey);
    if ca.kind==0 {
        println!("Not supported by library");
        return;
    }
    println!("Public key length= {}",ca.len);
    let pk=&cakey[0..ca.len];
    if !x509::self_signed(&certificate) {
        println!("Not self-signed");
    }
    if ca.kind==x509::PQ {
        printoct("Public key= \n",&pk);
    }

    println!("Extract secret key");
    let mut len=frombase64(&mut io,&SECRET);
    let mut ct=x509::extract_private_key(&io[0..len],&mut keys);

    printoct("Key pair= \n",&keys[0..ct.len]); 

    let sk=&keys[0..dilithium::SK_SIZE]; // access secret key (chop off public key)
    println!("Secret key length= {}",sk.len());
    printoct("Secret Key= \n",&sk); 
    println!("");

    let mut result=dilithium::verify(&pk,&certificate,&rs);
    if result {
        println!("Signature is verified");
    } else {
        println!("Signature is NOT verified");
    }

    let attempts=dilithium::signature(&sk,&certificate,&mut sig);
    println!("attempts= {}",attempts);
    let rs2=&sig[0..dilithium::SIG_SIZE];

    printoct("Signature= \n",&rs2);
    println!("Signature of length {}",slen);

    result=dilithium::verify(&pk,&certificate,&rs2);
    if result {
        println!("Signature is verified");
    } else {
        println!("Signature is NOT verified");
    }

 //   println!("\nChecking Self-Signed Signature");
 /*
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
    } */
} 

