from time import sleep
from random import randint
from threading import Thread, Semaphore

def produtor():
  global buffer
  for i in range(10):
    sleep(randint(0,2))           # fica um tempo produzindo...
    item = 'item ' + str(i)
    semaforo_vazio_buffer.acquire()  # verifica se há lugar no buffer
    semaforo_mutex.acquire()  # obtém acesso exclusivo ao buffer
    buffer.append(item)
    print('Produzido %s (ha %i itens no buffer)' % (item,len(buffer)))
    semaforo_mutex.release()  # libera o acesso ao buffer
    semaforo_item_no_buffer.release()  # notifica o consumidor que há um item disponível

def consumidor():
  global buffer
  for i in range(10):
    semaforo_item_no_buffer.acquire()  # aguarda que haja um item para consumir
    semaforo_mutex.acquire()  # obtém acesso exclusivo ao buffer
    item = buffer.pop(0)
    print('Consumido %s (ha %i itens no buffer)' % (item,len(buffer)))
    semaforo_mutex.release()  # libera o acesso ao buffer
    semaforo_vazio_buffer.release()  # incrementa o semáforo de slots disponíveis
    sleep(randint(0,2))         # fica um tempo consumindo...

buffer = []
tam_buffer = 3
# cria semáforos

# cria semáforos
semaforo_vazio_buffer = Semaphore(tam_buffer)  # semáforo para controlar o número de slots disponíveis no buffer
semaforo_mutex = Semaphore(1)  # semáforo para controlar o acesso exclusivo ao buffer
semaforo_item_no_buffer = Semaphore(0)  # semáforo para indicar se há um item disponível no buffer

produtor = Thread(target=produtor) 
consumidor = Thread(target=consumidor) 
produtor.start()
consumidor.start()
produtor.join()
consumidor.join() 