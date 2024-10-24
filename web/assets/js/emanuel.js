import { initializeApp } from "https://www.gstatic.com/firebasejs/9.4.0/firebase-app.js";
import { getFirestore, doc, collection, onSnapshot, query, where, orderBy, limit, getDocs, setDoc } from "https://www.gstatic.com/firebasejs/9.4.0/firebase-firestore.js";

const firebaseConfig = {
  apiKey: "AIzaSyA2nl8t6BDKGqyg_f1hxlRcC9v6DuwVkJg",
  authDomain: "saida-rapida.firebaseapp.com",
  projectId: "saida-rapida",
  storageBucket: "saida-rapida.appspot.com",
  messagingSenderId: "526559004556",
  appId: "1:526559004556:web:8fe92202bea42019e8b8a9"
};

const app = initializeApp(firebaseConfig);
const db = getFirestore();

const div = document.getElementById("estudantes")

const alunos = []

let unsubscribeAllStudents //Não tira, isso resolve um bug

class Consulta {
  constructor(IDEscola) {
    this.escola = doc(db, "escolas", IDEscola)
  }

  GetAllStudents() {
    unsubscribeAllStudents = onSnapshot(collection(this.escola, "alunos"), (querySnapshot) => {
      alunos.length = 0 //limpa o array

      querySnapshot.forEach((doc) => { //adiciona todos as pessoas que sao do nono ano no array
        alunos.push([doc.get("nome"), doc.id])
      });
      console.log(`Alunos salvos: ${alunos}`)
      this.GetStudents()
    })
    div.innerHTML = "" //Não tira, isso resolve um bug
  }

  async GetStudents() {
    const date = new Date
    alunos.forEach(async (aluno, index) => { //passa por cada aluno
      aluno.length = 2 //limpa o status na escola
      const q = query(collection(this.escola, "historico"), where("aluno", "==", aluno[1]), orderBy("hora", "desc"), limit(1)) //filtra e pega o salvo mais recente
      const querySnapshot = await getDocs(q) //cria um array com todos os docs

      if (!querySnapshot.empty) {
        const doc = querySnapshot.docs[0] //pega o doc mais recente

        const docHora = doc.get("hora")
        const docDate = docHora.toDate()
        console.log(docDate)
        console.log(date)

        if (date.getYear() == docDate.getYear() & date.getMonth() == docDate.getMonth() & date.getDate() == docDate.getDate()) {
          const status = doc.get("status")
          console.log(doc.id)
          console.log(status)

          /*if (status == "emTransicao")*/ aluno.push(status)
        }
        else console.log("O aluno não entrou hoje")
      }
      else console.log("O filtro tá vazio")

      console.log(aluno)
      console.log(alunos)

      if (alunos.length == index + 1) {
        const alunosComStatus = alunos.filter((alunos) => alunos[2] != undefined)
        this.atualizarAlunos(div, alunosComStatus)
      }
    })
  }

  atualizarAlunos(div, alunos) { //alunos[nome, id, status]
    div.innerHTML = ""
    alunos.sort()
    console.log(alunos)
    alunos.forEach((aluno) => {
      console.log(aluno)
      const p = document.createElement("p")

      p.innerText = aluno[0]
      p.className = aluno[2]

      div.appendChild(p)
      p.addEventListener("click", async (event) => {
        if (aluno[2] == "emTransicao") {
          this.alterarHistorico(aluno[1], "Deus")
        }
      })
    })
  }

  dateDate(IDAluno) {
    const year = new Date().getFullYear()
    const month = new Date().getMonth()
    const day = new Date().getDate()
    const hour = new Date().getHours()
    const minutes = new Date().getMinutes()
    const seconds = new Date().getSeconds()
    console.log(`dia: ${day}, mês: ${month}, ano: ${year}, hora: ${hour}, minuto ${minutes}, segundo: ${seconds}`)
    return `${day}${month}${year}${hour}${minutes}${seconds}${IDAluno}`
  }
  //Ezequi
  async alterarHistorico(alunoID, autorizadoPor) {
    await setDoc(doc(this.escola, "historico", this.dateDate(alunoID)), {
      aluno: alunoID,
      hora: new Date(),
      autorizadoPor: autorizadoPor,
      status: "foraDaEscola"
    });
    console.log("status atualizado")
  }
}
const consul = new Consulta("LFplnjWNSN0amNh0Da79")

consul.GetAllStudents()

const unsbscribedAtSchool = onSnapshot(collection(consul.escola, "historico"), (snapshot) => { //verifica se o historico foi alterado
  consul.GetStudents()
})
