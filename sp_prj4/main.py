import requests
from bs4 import BeautifulSoup

r = requests.get("http://cspro.sogang.ac.kr/~cse20121611")
soup = BeautifulSoup(r.content, "html.parser")
results = soup.find_all('a')

template = "cspro.sogang.ac.kr/~cse20121611"
URL_list = []
cnt = 2

URL = open("URL.txt","w")
URL.write(template+'\n'+template+'/0001.htm\n')

output = open("Output_0001.txt","w")
output.write(soup.get_text())
output.close()

for url_tag in results:
	temp_list = [url_tag]

	while temp_list:
		link_tag = temp_list.pop()
		link_name = link_tag.get('href')

		if not link_name:
			continue
		if (link_name.find("#")==0) or (link_name.find("?")==0):
			continue
		if (link_name == "http://"+template+"/index.html") or (link_name == "index.html"):
			continue
		if link_name in URL_list:
			continue

		page = requests.get("http://"+template+"/"+link_name)
		if not page.ok:
			continue

		output = open("Output_"+format(cnt,'04')+".txt","w")
		URL_list.append(link_name)
		URL.write(template+'/'+format(cnt,'04')+'.htm\n')

		soup = BeautifulSoup(page.content, "html.parser")
		output.write(soup.get_text())
		output.close()
		cnt += 1

		results2 = soup.find_all('a')
		results2.reverse()
		for url_temp in results2:
			temp_list.append(url_temp)

URL.close()
