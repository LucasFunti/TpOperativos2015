# tp-2015-2c-signiorcodigo


Secuencia de pedidos al administrador de memoria:

<h3>Si quieren cerrar su conexión</h3>

    Me llega: (de alguien)
        Código: -3 Cerrar socket de la conexión


<h3>Conexión inicial con el swap</h3>
    Envio: (al swap)
       Código: 1 soy el administrador y te estoy buscando
       
    Me llega: (del swap)
        Código: 2 Ok (como para confirmar)
  
<h3>INICIAR-N</h3>
     Me llega: (De la cpu)
        Código: 4
        Data: Dos enteros, primero el pid y luego la cantidad de páginas
      
    Envio: (al swap)
        Código: 4
        Data: Dos enteros, primero el pid y luego la cantidad de páginas
      
    Me llega: (del swap)
        Código: 1 la pude ubicar
        Código: -1 no lo pude ubicar
      
    Envio: (Al cpu)
        Código: 1 fué exitoso
        Código: 0 no se pudo ubicar
  
  
<h3>LEER-N</h3>
    Me llega:(del cpu)
        Código: 5 
        Data: Dos enteros, primero el pid y luego la página
      
    Envio: (al cpu)
        Código: 5 
        Data: Un string con el contenido
      
    Envio: (al swap en caso de no tenerlo) 
        Código: 5
        Data: Dos enteros, el pid y luego la página
      
      
<h3>ESCRIBIR-N</h3>
    Me llega: (del cpu)
      Código: 6 
      Data: Tres enteros y un String, primero el pid, luego la página, después el largo del string y por ultimo el string
      
    (No le respondo nada al cpu)
  
    Envio: (al swap en caso de tener que escribir)
      Código: 6
      Data: El pid, la página y el tamaño del string y el string
 
<h3>TERMINAR</h3>
 
    Me llega: (del cpu)
      Código: 8
      Data: Un entero, el pid
      
    (No le respondo nada al cpu)
  
    Envio: (al swap)
      Código: 8
      Data: El pid a borrar

      

  
  
  
