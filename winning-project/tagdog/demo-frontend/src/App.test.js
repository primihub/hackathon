import { render, screen } from '@testing-library/react';
import Stake from './pages/stake/Stake';

test('renders chain info', () => {
  render(<Stake />);
  const linkElement = screen.getByText(/Dapp connected to the/i);
  expect(linkElement).toBeInTheDocument();
});
