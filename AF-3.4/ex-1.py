from time import sleep
from random import randint
from threading import Thread, Lock, Condition

def produtor():
  global buffer
  for i in range(10):
    sleep(randint(0,2))           # fica um tempo produzindo...
    item = 'item ' + str(i)
    with lock:
      if len(buffer) == tam_buffer:
        print('>>> Buffer cheio. Produtor ira aguardar.')
        lugar_no_buffer.wait()    # aguarda que haja lugar no buffer
      buffer.append(item)
      print('Produzido %s (ha %i itens no buffer)' % (item,len(buffer)))
      item_no_buffer.notify()

def consumidor():
  global buffer
  for i in range(10):
    with lock:
      if len(buffer) == 0:
        print('>>> Buffer vazio. Consumidor ira aguardar.')
        item_no_buffer.wait()   # aguarda que haja um item para consumir 
      item = buffer.pop(0)
      print('Consumido %s (ha %i itens no buffer)' % (item,len(buffer)))
      lugar_no_buffer.notify()
    sleep(randint(0,2))         # fica um tempo consumindo...

buffer = []
tam_buffer = 5
lock = Lock()
lugar_no_buffer = Condition(lock)
item_no_buffer = Condition(lock)

produtores_threads = []
consumidores_threads = []

produtores_threads.append(Thread(target=produtor))
produtores_threads.append(Thread(target=produtor))
consumidores_threads.append(Thread(target=consumidor))
consumidores_threads.append(Thread(target=consumidor))

for produtor_thread in produtores_threads:
    produtor_thread.start()

for consumidor_thread in consumidores_threads:
    consumidor_thread.start()

for produtor_thread in produtores_threads:
    produtor_thread.join()

for consumidor_thread in consumidores_threads:
    consumidor_thread.join()