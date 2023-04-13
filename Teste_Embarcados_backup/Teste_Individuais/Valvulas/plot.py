import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import style

df = pd.read_csv('plot.csv',';')
plt.style.use('ggplot')
plt.plot(df['Carga'])
plt.title('Controle de Carga - Teste das Válvulas')
plt.xlabel('Tempo (ms)')
plt.ylabel('Carga')
plt.show()
