import Vue from 'vue'
import App from './App.vue'
import router from './router'
import { BootstrapVue, NavbarPlugin } from 'bootstrap-vue'

import 'bootstrap/dist/css/bootstrap.css'
import 'bootstrap-vue/dist/bootstrap-vue.css'

import '@/styles/style.scss'

// Install BootstrapVue
Vue.use(BootstrapVue)
Vue.use(NavbarPlugin)

Vue.config.productionTip = false

new Vue({
  router,
  render: h => h(App)
}).$mount('#app')
